#ifndef AMALGAMATION
#include <dlfcn.h>              // dl*
#endif
#include <sstream>
#include <string>
#include <vector>

#include "Logger/Logger.h"
#include "storage/StorageHandler.h"
#include "util.h"

using datamodel::Accessory;
using datamodel::Track;
using datamodel::Feedback;
using datamodel::Layer;
using datamodel::Loco;
using datamodel::Relation;
using datamodel::Street;
using datamodel::Switch;
using std::map;
using std::string;
using std::vector;

namespace storage {

	StorageHandler::StorageHandler(Manager* manager, const StorageParams& params)
	:	manager(manager),
		createStorage(nullptr),
		destroyStorage(nullptr),
		instance(nullptr),
		dlhandle(nullptr),
		transactionRunning(false)
	{
#ifdef AMALGAMATION
		createStorage = (storage::StorageInterface* (*)(storage::StorageParams))(&create_sqlite);
		destroyStorage = (void (*)(storage::StorageInterface*))(&destroy_sqlite);
#else
		// generate symbol and library names
		char* error;
		std::stringstream ss;
		ss << "storage/" << params.module << ".so";

		Logger::Logger* logger = Logger::Logger::GetLogger("StorageHandler");
		dlhandle = dlopen(ss.str().c_str(), RTLD_LAZY);
		if (!dlhandle)
		{
			logger->Error("Can not open storage library: {0}", dlerror());
			return;
		}

		// look for symbol create_*
		ss.str(std::string());
		ss << "create_" << params.module;
		string s = ss.str();
		createStorage_t* newCreateStorage = (createStorage_t*) dlsym(dlhandle, s.c_str());
		error = dlerror();
		if (error)
		{
			logger->Error("Unable to find symbol {0}", s);
			return;
		}

		// look for symbol destroy_*
		ss.str(std::string());
		ss << "destroy_" << params.module;
		s = ss.str();
		destroyStorage_t* newDestroyStorage = (destroyStorage_t*) dlsym(dlhandle, s.c_str());
		error = dlerror();
		if (error)
		{
			logger->Error("Unable to find symbol {0}", s);
			return;
		}

		// register  valid symbols
		createStorage = newCreateStorage;
		destroyStorage = newDestroyStorage;
#endif

		// start storage
		if (createStorage)
		{
			instance = createStorage(params);
		}
	}

	StorageHandler::~StorageHandler()
	{
		// stop storage
		if (instance)
		{
			destroyStorage(instance);
			instance = NULL;
		}

#ifndef AMALGAMATION
		// close library
		if (dlhandle)
		{
			dlclose(dlhandle);
			dlhandle = NULL;
		}
#endif
	}

	void StorageHandler::Save(const hardware::HardwareParams& hardwareParams)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->SaveHardwareParams(hardwareParams);
		CommitTransactionInternal();
	}

	void StorageHandler::AllHardwareParams(std::map<controlID_t,hardware::HardwareParams*>& hardwareParams)
	{
		if (instance == nullptr)
		{
			return;
		}
		instance->AllHardwareParams(hardwareParams);
	}

	void StorageHandler::DeleteHardwareParams(const controlID_t controlID)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->DeleteHardwareParams(controlID);
		CommitTransactionInternal();
	}

	void StorageHandler::AllLocos(map<locoID_t,datamodel::Loco*>& locos)
	{
		if (instance == nullptr)
		{
			return;
		}
		vector<string> objects;
		instance->ObjectsOfType(ObjectTypeLoco, objects);
		for(auto object : objects) {
			Loco* loco = new Loco(manager, object);
			locos[loco->GetID()] = loco;
		}
	}

	void StorageHandler::DeleteLoco(const locoID_t locoID)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->DeleteObject(ObjectTypeLoco, locoID);
		CommitTransactionInternal();
	}

	void StorageHandler::AllAccessories(std::map<accessoryID_t,datamodel::Accessory*>& accessories)
	{
		if (instance == nullptr)
		{
			return;
		}
		vector<string> objects;
		instance->ObjectsOfType(ObjectTypeAccessory, objects);
		for(auto object : objects)
		{
			Accessory* accessory = new Accessory(object);
			accessories[accessory->GetID()] = accessory;
		}
	}

	void StorageHandler::DeleteAccessory(const accessoryID_t accessoryID)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->DeleteObject(ObjectTypeAccessory, accessoryID);
		CommitTransactionInternal();
	}

	void StorageHandler::AllFeedbacks(std::map<feedbackID_t,datamodel::Feedback*>& feedbacks)
	{
		if (instance == nullptr)
		{
			return;
		}
		vector<string> objects;
		instance->ObjectsOfType(ObjectTypeFeedback, objects);
		for(auto object : objects)
		{
			Feedback* feedback = new Feedback(manager, object);
			feedbacks[feedback->GetID()] = feedback;
		}
	}

	void StorageHandler::DeleteFeedback(const feedbackID_t feedbackID)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->DeleteObject(ObjectTypeFeedback, feedbackID);
		CommitTransactionInternal();
	}

	void StorageHandler::AllTracks(std::map<trackID_t,datamodel::Track*>& tracks)
	{
		if (instance == nullptr)
		{
			return;
		}
		vector<string> objects;
		instance->ObjectsOfType(ObjectTypeTrack, objects);
		for(auto object : objects)
		{
			Track* track = new Track(manager, object);
			tracks[track->GetID()] = track;
		}
	}

	void StorageHandler::DeleteTrack(const trackID_t trackID)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->DeleteObject(ObjectTypeTrack, trackID);
		CommitTransactionInternal();
	}

	void StorageHandler::AllSwitches(std::map<switchID_t,datamodel::Switch*>& switches)
	{
		if (instance == nullptr)
		{
			return;
		}
		vector<string> objects;
		instance->ObjectsOfType(ObjectTypeSwitch, objects);
		for(auto object : objects)
		{
			Switch* mySwitch = new Switch(object);
			switches[mySwitch->GetID()] = mySwitch;
		}
	}

	void StorageHandler::DeleteSwitch(const switchID_t switchID)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->DeleteObject(ObjectTypeSwitch, switchID);
		CommitTransactionInternal();
	}

	void StorageHandler::Save(const datamodel::Street& street)
	{
		if (instance == nullptr)
		{
			return;
		}
		string serialized = street.Serialize();
		StartTransactionInternal();
		const streetID_t streetID = street.GetID();
		instance->SaveObject(ObjectTypeStreet, streetID, street.GetName(), serialized);
		instance->DeleteRelationFrom(ObjectTypeStreet, streetID);
		const vector<datamodel::Relation*> relations = street.GetRelations();
		for (auto relation : relations)
		{
			string serializedRelation = relation->Serialize();
			instance->SaveRelation(ObjectTypeStreet, streetID, relation->ObjectType2(), relation->ObjectID2(), relation->Priority(), serializedRelation);
		}
		CommitTransactionInternal();
	}

	void StorageHandler::AllStreets(std::map<streetID_t,datamodel::Street*>& streets)
	{
		if (instance == nullptr)
		{
			return;
		}
		vector<string> objects;
		instance->ObjectsOfType(ObjectTypeStreet, objects);
		for (auto object : objects) {
			Street* street = new Street(manager, object);
			vector<string> relationsString;
			const streetID_t streetID = street->GetID();
			instance->RelationsFrom(ObjectTypeStreet, streetID, relationsString);
			vector<Relation*> relations;
			for (auto relationString : relationsString)
			{
				relations.push_back(new Relation(manager, relationString));
			}
			street->AssignRelations(relations);
			streets[streetID] = street;
		}
	}

	void StorageHandler::DeleteStreet(const streetID_t streetID)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->DeleteObject(ObjectTypeStreet, streetID);
		CommitTransactionInternal();
	}

	void StorageHandler::AllLayers(std::map<layerID_t,datamodel::Layer*>& layers)
	{
		if (instance == nullptr)
		{
			return;
		}
		vector<string> objects;
		instance->ObjectsOfType(ObjectTypeLayer, objects);
		for(auto object : objects) {
			Layer* layer = new Layer(object);
			layers[layer->GetID()] = layer;
		}
	}

	void StorageHandler::DeleteLayer(const layerID_t layerID)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->DeleteObject(ObjectTypeLayer, layerID);
		CommitTransactionInternal();
	}

	void StorageHandler::SaveSetting(const std::string& key, const std::string& value)
	{
		if (instance == nullptr)
		{
			return;
		}
		StartTransactionInternal();
		instance->SaveSetting(key, value);
		CommitTransactionInternal();
	}

	std::string StorageHandler::GetSetting(const std::string& key)
	{
		if (instance == nullptr)
		{
			return "";
		}
		return instance->GetSetting(key);
	}

	void StorageHandler::StartTransaction()
	{
		if (instance == nullptr)
		{
			return;
		}
		transactionRunning = true;
		instance->StartTransaction();
	}

	void StorageHandler::CommitTransaction()
	{
		if (instance == nullptr)
		{
			return;
		}
		transactionRunning = false;
		instance->CommitTransaction();
	}

	void StorageHandler::StartTransactionInternal()
	{
		if (transactionRunning == true)
		{
			return;
		}
		instance->StartTransaction();
	}

	void StorageHandler::CommitTransactionInternal()
	{
		if (transactionRunning == true)
		{
			return;
		}
		instance->CommitTransaction();
	}
} // namespace storage

