#pragma once

#include <string>

#include "command_interface.h"
#include "datatypes.h"
#include "hardware/HardwareInterface.h"
#include "hardware/HardwareParams.h"
#include "manager.h"
#include "util.h"

namespace hardware {

	// the types of the class factories
	typedef hardware::HardwareInterface* createHardware_t(const hardware::HardwareParams* params);
	typedef void destroyHardware_t(hardware::HardwareInterface*);

	class HardwareHandler: public CommandInterface {
		public:
			HardwareHandler(Manager& manager, const HardwareParams* params);
			~HardwareHandler();
			controlID_t getControlID() const { return params->controlID; }
			const std::string getName() const override;

			void booster(const controlType_t managerID, boosterStatus_t status) override;
			void locoSpeed(const controlType_t managerID, const locoID_t locoID, const speed_t speed) override;
			void locoDirection(const controlType_t managerID, const locoID_t locoID, const direction_t direction) override;
			void locoFunction(const controlType_t managerID, const locoID_t locoID, const function_t function, const bool on) override;
			void accessory(const controlType_t managerID, const accessoryID_t accessoryID, const accessoryState_t state) override;
			void feedback(const controlType_t managerID, const feedbackPin_t pin, const feedbackState_t state) override {};
			void block(const controlType_t managerID, const blockID_t blockID, const lockState_t state) override {};
			void handleSwitch(const controlType_t managerID, const switchID_t switchID, const switchState_t state) override;
			void locoIntoBlock(const locoID_t locoID, const blockID_t blockID) override {};
			void locoRelease(const locoID_t) override {};
			void blockRelease(const blockID_t) override {};
			void streetRelease(const streetID_t) override {};
			void locoStreet(const locoID_t locoID, const streetID_t streetID, const blockID_t blockID) override {};
			void locoDestinationReached(const locoID_t locoID, const streetID_t streetID, const blockID_t blockID) override {};
			void locoStart(const locoID_t locoID) override {};
			void locoStop(const locoID_t locoID) override {};
			void getProtocols(std::vector<protocol_t>& protocols) const override;
			bool protocolSupported(protocol_t protocol) const override;
		private:
			Manager& manager;
			createHardware_t* createHardware;
			destroyHardware_t* destroyHardware;
			hardware::HardwareInterface* instance;
			const HardwareParams* params;
	};

}; // namespace hardware
