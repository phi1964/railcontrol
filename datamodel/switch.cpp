#include "switch.h"

namespace datamodel {

	Switch::Switch(switchID_t switchID,
	               std::string name,
								 controlID_t controlID,
								 protocol_t protocol,
								 address_t address,
								 switchType_t type,
								 switchState_t state,
								 layoutRotation_t rotation,
								 layoutPosition_t x,
								 layoutPosition_t y,
								 layoutPosition_t z) :
		Accessory(switchID, name, controlID, protocol, address, type, state, x, y, z),
		switchID(switchID),
		rotation(rotation) {
	}

} // namespace datamodel

