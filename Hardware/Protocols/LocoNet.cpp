/*
RailControl - Model Railway Control Software

Copyright (c) 2017-2022 Dominik (Teddy) Mahrer - www.railcontrol.org

RailControl is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

RailControl is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RailControl; see the file LICENCE. If not see
<http://www.gnu.org/licenses/>.
*/

#include <string>
#include <thread>
#include "Hardware/Protocols/LocoNet.h"
#include "Utils/Utils.h"

namespace Hardware
{
	namespace Protocols
	{
		LocoNet::LocoNet(const HardwareParams* params,
			const std::string& controlName,
			const unsigned int dataSpeed)
		:	HardwareInterface(params->GetManager(),
			   params->GetControlID(),
				controlName + " / " + params->GetName() + " at serial port " + params->GetArg1(),
			   params->GetName()),
			run(true),
			serialLine(logger, params->GetArg1(), dataSpeed, 8, 'N', 1)
		{
			receiverThread = std::thread(&Hardware::Protocols::LocoNet::Receiver, this);

			Utils::Utils::SleepForSeconds(1);

			unsigned char buffer[4];
			buffer[0] = OPC_RQ_SL_DATA;
			buffer[2] = 0;
			for (unsigned char slot = LocoNetLocoCache::MinLocoNetSlot; slot <= LocoNetLocoCache::MaxLocoNetSlot; ++slot)
			{
				Utils::Utils::SleepForMilliseconds(25);
				buffer[1] = slot;
				CalcCheckSum(buffer, 3, buffer + 3);
				logger->Hex(buffer, sizeof(buffer));
				serialLine.Send(buffer, sizeof(buffer));
			}
		}

		LocoNet::~LocoNet()
		{
			run = false;
			receiverThread.join();
			logger->Info(Languages::TextTerminatingSenderSocket);
		}

		void LocoNet::Booster(const BoosterState status)
		{
			unsigned char buffer[2];
			if (status)
			{
				buffer[0] = OPC_GPON;
				logger->Info(Languages::TextTurningBoosterOn);
			}
			else
			{
				buffer[0] = OPC_GPOFF;
				logger->Info(Languages::TextTurningBoosterOff);
			}
			CalcCheckSum(buffer, 1, buffer + 1);
			logger->Hex(buffer, sizeof(buffer));
			serialLine.Send(buffer, sizeof(buffer));
		}

		void LocoNet::AccessoryOnOrOff(__attribute__((unused)) const Protocol protocol,
			const Address address,
			const DataModel::AccessoryState state,
			const bool on)
		{
			logger->Info(Languages::TextSettingAccessory, address, Languages::GetGreenRed(state), Languages::GetOnOff(on));
			unsigned char buffer[4];
			buffer[0] = OPC_SW_REQ;
			const Address addressLocoNet = address - 1;
			buffer[1] = static_cast<unsigned char>(addressLocoNet & 0x007F);
			buffer[2] = static_cast<unsigned char>(((addressLocoNet >> 7) & 0x000F) | ((state & 0x01) << 5) | ((on & 0x01) << 4));
			CalcCheckSum(buffer, 3, buffer + 3);
			logger->Hex(buffer, sizeof(buffer));
			serialLine.Send(buffer, sizeof(buffer));
		}

		void LocoNet::Receiver()
		{
			Utils::Utils::SetThreadName("LocoNet Receiver");
			logger->Info(Languages::TextReceiverThreadStarted);

			const unsigned char BufferSize = 128u;
			unsigned char buffer[BufferSize];
			unsigned char checkSumCalculated;
			unsigned char commandLength;
			while (run)
			{
				if (!serialLine.IsConnected())
				{
					logger->Error(Languages::TextUnableToReceiveData);
					return;
				}
				ssize_t dataLength = serialLine.ReceiveExact(buffer, 1);
				if (dataLength != 1)
				{
					continue;
				}
				unsigned char commandType = buffer[0] & 0xE0;
				switch (commandType)
				{
					case 0x80:
						dataLength = serialLine.ReceiveExact(buffer + 1, 1);
						if (dataLength != 1)
						{
							continue;
						}
						commandLength = 2;
						break;

					case 0xA0:
					{
						dataLength = serialLine.ReceiveExact(buffer + 1, 3);
						if (dataLength != 3)
						{
							continue;
						}
						commandLength = 4;
						break;
					}

					case 0xC0:
					{
						dataLength = serialLine.ReceiveExact(buffer + 1, 5);
						if (dataLength != 5)
						{
							continue;
						}
						commandLength = 6;
						break;
					}

					case 0xE0:
					{
						dataLength = serialLine.ReceiveExact(buffer + 1, 1);
						if (dataLength != 1)
						{
							continue;
						}
						unsigned char dataSize = buffer[1];
						dataLength = serialLine.ReceiveExact(buffer + 2, dataSize + 1);
						if (dataLength != dataSize)
						{
							continue;
						}
						commandLength = dataSize + 2;
						break;
					}

					default:
						continue;
				}
				logger->Hex(buffer, commandLength);
				CalcCheckSum(buffer, commandLength - 1, &checkSumCalculated);
				if (checkSumCalculated != buffer[commandLength - 1])
				{
					continue;
				}

				if (run == false)
				{
					break;
				}

				Parse(buffer);
			}
			logger->Info(Languages::TextTerminatingReceiverThread);
		}

		void LocoNet::CalcCheckSum(unsigned char* data, const unsigned char length, unsigned char* checkSum)
		{
			*checkSum = 0xFF;
			for (unsigned char i = 0; i < length; ++i)
			{
				(*checkSum) ^= data[i];
			}
		}

		void LocoNet::Parse(unsigned char* data)
		{
			switch (data[0])
			{
				case OPC_GPON:
					logger->Info(Languages::TextTurningBoosterOn);
					manager->Booster(ControlTypeHardware, BoosterStateGo);
					break;

				case OPC_GPOFF:
					logger->Info(Languages::TextTurningBoosterOff);
					manager->Booster(ControlTypeHardware, BoosterStateStop);
					break;

				case OPC_SW_REQ:
				{
					const bool on = static_cast<bool>((data[2] & 0x10) >> 4);
					if (!on)
					{
						break;
					}
					const DataModel::AccessoryState state = static_cast<DataModel::AccessoryState>((data[2] & 0x20) >> 5);
					const Address address = (static_cast<Address>(data[1] & 0x7F) | (static_cast<Address>(data[2] & 0x0F) << 7)) + 1;
					logger->Info(Languages::TextSettingAccessory, address, Languages::GetGreenRed(state), Languages::GetOnOff(on));
					manager->AccessoryState(ControlTypeHardware, ControlID(), ProtocolServer, address, state);
					break;
				}

				case OPC_SL_RD_DATA:
				{
					if (data[1] != 0x0E)
					{
						break;
					}
					const unsigned char slot = data[2];
					const Address address = static_cast<Address>(data[4] & 0x7F) | (static_cast<Address>(data[9] & 0x3F) << 7);
					logger->Debug("Slot {0} has address {1}", slot, address);
					locoCache.Set(slot, address);
					break;
				}

				default:
					break;
			}
		}
	} // namespace
} // namespace
