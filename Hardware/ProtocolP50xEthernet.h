/*
RailControl - Model Railway Control Software

Copyright (c) 2017-2021 Dominik (Teddy) Mahrer - www.railcontrol.org

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

#pragma once

#include <string>

#include "Hardware/ProtocolP50x.h"
#include "Logger/Logger.h"
#include "Network/TcpClient.h"

namespace Hardware
{
	class HardwareParams;

	class ProtocolP50xEthernet : public ProtocolP50x
	{
		public:
			ProtocolP50xEthernet() = delete;
			ProtocolP50xEthernet(const ProtocolP50xEthernet&) = delete;
			ProtocolP50xEthernet& operator=(const ProtocolP50xEthernet&) = delete;

			inline ProtocolP50xEthernet(const HardwareParams* params,
				const std::string& controlName,
				const ProtocolP50xType type)
			:	ProtocolP50x(params,
					controlName + " / " + params->GetName() + " at serial port " + params->GetArg1(),
					type),
				connection(Network::TcpClient::GetTcpClientConnection(logger, params->GetArg1(), MasterControl2Port))
			{
			}

			virtual ~ProtocolP50xEthernet()
			{
			}

		protected:
			inline int Send(const unsigned char* data, const size_t length) const override
			{
				return connection.Send(data, length);
			}

			inline ssize_t Receive(unsigned char* data, const size_t length) const override
			{
				return connection.Receive(data, length);
			}

			inline ssize_t ReceiveExact(unsigned char* data, const size_t length) const override
			{
				return connection.ReceiveExact(data, length);
			}

		private:
			static const unsigned short MasterControl2Port = 8050;

			Network::TcpConnection connection;
	};
} // namespace

