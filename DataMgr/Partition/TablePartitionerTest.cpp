/**
 * @file	TablePartitionerTest 
 * @author	Todd Mostak <todd@map-d.com>
 */

#include "PartitionIncludes.h"
#include "TablePartitionMgr.h"
#include "../File/FileMgr.h"
#include "../Buffer/BufferMgr.h"
#include "../Metadata/Catalog.h"

#include "../../Shared/errors.h"
#include "../../Shared/ansi.h"
#include "../../Shared/testing.h"
#include "../../Shared/types.h"

#include <stdlib.h>

using namespace Testing;
using namespace std;

namespace Partition_Namespace { 

class TablePartitionerTest { 


    public:

        ~TablePartitionerTest() {
            delete tablePartitionMgr_;
            delete catalog_;
            delete bufferMgr_;
            delete fileMgr_;
        }

        void reset() {
            system("dropdb mapd");
            system("createdb mapd");
            system("rm -rf data");
            system("mkdir data");
        }

        bool instanciate() {
            fileMgr_ = new File_Namespace::FileMgr ("data");
            bufferMgr_ = new Buffer_Namespace::BufferMgr (128*1048576, fileMgr_);
            catalog_ = new Metadata_Namespace::Catalog ("data");
            tablePartitionMgr_ = new TablePartitionMgr (*catalog_, *bufferMgr_);
            Testing::pass++;
            return true;
        }

        bool createTableAndPartitioners() {
            vector <Metadata_Namespace::ColumnRow *> columnRows;
            columnRows.push_back(new Metadata_Namespace::ColumnRow("a", INT_TYPE, true));
            columnRows.push_back(new Metadata_Namespace::ColumnRow("b", INT_TYPE, false));
            columnRows.push_back(new Metadata_Namespace::ColumnRow("c", FLOAT_TYPE, false));
            mapd_err_t status = catalog_ -> addTableWithColumns("test1", columnRows);
            if (status != MAPD_SUCCESS) 
                return false;
            tablePartitionMgr_ -> createPartitionerForTable("test1",LINEAR);
            Testing::pass++;

            // now add another
            tablePartitionMgr_ -> createPartitionerForTable("test1",LINEAR);
            Metadata_Namespace::TableRow tableRow;
            status = catalog_ -> getMetadataForTable("test1",tableRow);
            if (status != MAPD_SUCCESS)
                return false;
            int tableId = tableRow.tableId; 
            auto tableIt = tablePartitionMgr_ -> tableToPartitionerMap_.find(tableId);
            if (tableIt == tablePartitionMgr_ -> tableToPartitionerMap_.end())
                return false;
            Testing::pass++;
            if (tableIt -> second.size() != 2)
                return false;
            Testing::pass++;
            // Verify partitioner ids are different
            int partId1 = tableIt -> second[0] -> getPartitionerId();
            int partId2 = tableIt -> second[1] -> getPartitionerId();
            if (partId1 == partId2)
                return false;
            Testing::pass++;
            // Verify both partitioners are linear
            string partType1 = tableIt -> second[0] -> getPartitionerType();
            string partType2 = tableIt -> second[1] -> getPartitionerType();
            if (partType1 != "linear" || partType2 != "linear")
                return false;
            Testing::pass++;
            return true;
    }

    bool insertIntoPartitioners() {
        vector <string> columnNames = {"a", "b", "c"};

       
        vector <int> columnIds; 
        vector <void *> data;
        columnIds.push_back(0);
        columnIds.push_back(1);
        columnIds.push_back(2);
        int intData = 3;
        return true;

    }

    
    private:
        File_Namespace::FileMgr *fileMgr_;
        Buffer_Namespace::BufferMgr * bufferMgr_;
        Metadata_Namespace::Catalog * catalog_;
        TablePartitionMgr * tablePartitionMgr_;
};



} // Partition_Namespace

int main(void) {
    Partition_Namespace::TablePartitionerTest tablePartitionerTest;
    tablePartitionerTest.reset();
    tablePartitionerTest.instanciate() ?
        PPASS("Instanciate") : PFAIL("Instanciate");
    tablePartitionerTest.createTableAndPartitioners() ?
        PPASS("Create Table and partitioners") : PFAIL("Create Table and partitioners");
    return 0;
}
