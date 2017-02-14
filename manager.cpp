#include <iostream>

#include "manager.h"

#include "hardware/hardware_handler.h"
#include "hardware/hardware_params.h"
#include "util.h"
#include "webserver/webserver.h"

using datamodel::Loco;
using hardware::HardwareHandler;
using hardware::HardwareParams;
using storage::StorageHandler;
using storage::StorageParams;
using webserver::WebServer;

Manager::Manager() :
	storage(NULL) {

  controllers.push_back(new WebServer(*this, 8080));

	HardwareParams hardwareParamsVirt;
	hardwareParamsVirt.name = "Virtuelle Zentrale";
	hardwareParamsVirt.ip = "";
	controlID_t nextControlID = 0;
	controllers.push_back(new HardwareHandler(*this, HARDWARE_ID_VIRT, nextControlID++, hardwareParamsVirt));

	StorageParams storageParams;
	storageParams.module = "sqlite";
	storageParams.filename = "/tmp/railcontrol.db";
	storage = new StorageHandler(storageParams);

	Loco newloco1(1, "My Loco", 4, 1200);
	storage->loco(newloco1);

	Loco newloco2(2, "Your Loco", 4, 1201);
	storage->loco(newloco2);

	storage->allLocos(locos);
	for (auto loco : locos) {
		xlog("Loaded loco %s", loco.second->name.c_str());
	}
}

Manager::~Manager() {
  for (auto control : controllers) {
    delete control;
  }
	for (auto loco : locos) {
		delete loco.second;
	}
	delete storage;
	storage = NULL;
}

void Manager::go(const managerID_t controlID) {
  for (auto control : controllers) {
		control->go(controlID);
	}
}

void Manager::stop(const managerID_t controlID) {
  for (auto control : controllers) {
		control->stop(controlID);
	}
}

bool Manager::getProtocolAddress(const locoID_t locoID, controlID_t& controlID, protocol_t& protocol, address_t& address) const {
	if (locos.count(locoID) < 1) {
		controlID = 0;
		protocol = PROTOCOL_NONE;
		address = 0;
		return false;
	}
	Loco* loco = locos.at(locoID);
	controlID = loco->controlID;
	protocol = loco->protocol;
	address = loco->address;
	return true;
}

void Manager::locoSpeed(const managerID_t controlID, const locoID_t locoID, const speed_t speed) {
  for (auto control : controllers) {
    control->locoSpeed(controlID, locoID, speed);
  }
}

const std::map<locoID_t,datamodel::Loco*>& Manager::locoList() const {
	return locos;
}