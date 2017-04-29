#ifndef DATAMODEL_BLOCK_H
#define DATAMODEL_BLOCK_H

#include <mutex>
#include <string>

#include "datatypes.h"
#include "layout_item.h"
#include "serializable.h"

namespace datamodel {

	class Block : public LayoutItem {
		public:
			Block(const blockID_t blockID, const std::string& name, const layoutItemSize_t width, const layoutRotation_t rotation, const layoutPosition_t x, const layoutPosition_t y, const layoutPosition_t z);
			Block(const std::string& serialized);

			std::string serialize() const override;
			bool deserialize(const std::string& serialized) override;

			bool tryReserve(const locoID_t locoID);
			bool reserve(const locoID_t locoID);
			bool free(const locoID_t locoID);

			static void getTexts(const blockState_t state, char*& stateText);

		private:
			blockState_t state;
			locoID_t locoID;
			std::mutex updateMutex;
	};

} // namespace datamodel

#endif // DATAMODEL_BLOCK_H
