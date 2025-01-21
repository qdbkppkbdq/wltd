#include "datastore.h"

class FileDataStore : public DataStore {
public:
    FileDataStore() {}
    ~FileDataStore() {}
    TraderResult Init(void* config) override;
    TraderResult Start() override;
    TraderResult Stop() override;
    TraderResult loadDataToMemory(void* fmt) override;
    TraderResult storeData(dataType data) override;
    TraderResult updateData(dataType data) override;
};