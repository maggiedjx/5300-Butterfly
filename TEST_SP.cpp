// TODO Header
//


#include <iostream>
#include <string>
#include <cstring>
#include <utility>

#include "db_cxx.h"

#include "storage_engine.h"
#include "heap_storage.h"

// Silence some issues w/ including heap_storage
DbEnv* _DB_ENV;

int main() {

    // Not rows, test with a simpler data type to start:
    // Test data strings
    std::string test_strings[] {
        "lorem",
        "ipsum",
        "dolor"
    };
    const size_t NUM_TEST_DATAS = 3;

    // Test data dbt structs
    Dbt test_data_dbts[NUM_TEST_DATAS];
    for(size_t i = 0; i < NUM_TEST_DATAS; ++i) {
        void* bits_i = (void*)(test_strings[i].c_str()); 
        test_data_dbts[i] = Dbt(bits_i,sizeof(bits_i));
    }

    // Make sure Dbts constructed well
    std::cout << "Data in a Dbt: " << (char*)test_data_dbts[2].get_data() << std::endl;

    u_int32_t blockID = 1;

    // Create a sloted page
    char bits1[4096/8];
    Dbt _sp_1_backing(bits1,sizeof(bits1));
    
    SlottedPage test_spage_1(_sp_1_backing,blockID);

    // Add to slotted page
    test_spage_1.add(&test_data_dbts[0]);
    test_spage_1.add(&test_data_dbts[1]);


}
