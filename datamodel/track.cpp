#include <algorithm>
#include <map>
#include <sstream>
#include <string>

#include <datamodel/feedback.h>
#include <datamodel/track.h>
#include <manager.h>

using std::map;
using std::string;
using std::vector;

namespace datamodel
{
	std::string Track::Serialize() const
	{
		std::string feedbackString;
		for (auto feedback : feedbacks)
		{
			if (feedbackString.size() > 0)
			{
				feedbackString += ",";
			}
			feedbackString += std::to_string(feedback);
		}
		std::stringstream ss;
		ss << "objectType=Track;"
			<< LayoutItem::Serialize()
			<< LockableItem::Serialize()
			<< ";type=" << static_cast<int>(type)
			<< ";feedbacks=" << feedbackString
			<< ";state=" << static_cast<int>(state)
			<< ";locoDirection=" << static_cast<int>(locoDirection);
		return ss.str();
	}

	bool Track::Deserialize(const std::string& serialized)
	{
		map<string, string> arguments;
		ParseArguments(serialized, arguments);
		LayoutItem::Deserialize(arguments);
		LockableItem::Deserialize(arguments);
		width = Width1;
		visible = VisibleYes;
		string objectType = GetStringMapEntry(arguments, "objectType");
		if (objectType.compare("Track") != 0)
		{
			return false;
		}
		type = static_cast<trackType_t>(GetBoolMapEntry(arguments, "type", TrackTypeStraight));
		string feedbackStrings = GetStringMapEntry(arguments, "feedbacks");
		vector<string> feedbackStringVector;
		str_split(feedbackStrings, ",", feedbackStringVector);
		for (auto feedbackString : feedbackStringVector)
		{
			feedbacks.push_back(Util::StringToInteger(feedbackString));
		}
		state = static_cast<feedbackState_t>(GetBoolMapEntry(arguments, "state", FeedbackStateFree));
		locoDirection = static_cast<direction_t>(GetBoolMapEntry(arguments, "locoDirection", DirectionLeft));
		return true;
	}

	bool Track::FeedbackState(const feedbackID_t feedbackID, const feedbackState_t state)
	{
		bool ret = FeedbackStateInternal(feedbackID, state);
		manager->TrackPublishState(this);
		return ret;
	}

	bool Track::FeedbackStateInternal(const feedbackID_t feedbackID, const feedbackState_t state)
	{

		Loco* loco = manager->GetLoco(GetLoco());
		if (loco != nullptr && state == FeedbackStateOccupied)
		{
			loco->DestinationReached(feedbackID);
		}

		std::lock_guard<std::mutex> Guard(updateMutex);
		if (state != FeedbackStateFree)
		{
			this->state = state;
			return true;
		}
		for (auto f : feedbacks)
		{
			datamodel::Feedback* feedback = manager->GetFeedback(f);
			if (feedback == nullptr)
			{
				continue;
			}
			if (feedback->GetState() != FeedbackStateFree)
			{
				return false;
			}
		}
		this->state = FeedbackStateFree;
		return true;
	}

	bool Track::AddStreet(Street* street)
	{
		std::lock_guard<std::mutex> Guard(updateMutex);
		for (auto s : streets)
		{
			if (s == street)
			{
				return false;
			}
		}
		streets.push_back(street);
		return true;
	}

	bool Track::RemoveStreet(Street* street)
	{
		std::lock_guard<std::mutex> Guard(updateMutex);
		size_t sizeBefore = streets.size();
		streets.erase(std::remove(streets.begin(), streets.end(), street), streets.end());
		size_t sizeAfter = streets.size();
		return sizeBefore > sizeAfter;
	}

	bool Track::ValidStreets(std::vector<Street*>& validStreets)
	{
		std::lock_guard<std::mutex> Guard(updateMutex);
		for (auto street : streets)
		{
			if (street->FromTrackDirection(objectID, locoDirection))
			{
				validStreets.push_back(street);
			}
		}
		return true;
	}
} // namespace datamodel
