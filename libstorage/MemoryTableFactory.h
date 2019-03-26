/*
 * @CopyRight:
 * FISCO-BCOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FISCO-BCOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FISCO-BCOS.  If not, see <http://www.gnu.org/licenses/>
 * (c) 2016-2018 fisco-dev contributors.
 */
/** @file MemoryTableFactory.h
 *  @author ancelmo
 *  @date 20180921
 */
#pragma once

#include "Common.h"
#include "MemoryTable.h"
#include "Storage.h"
#include "Table.h"
#include "TablePrecompiled.h"
#include <libdevcore/easylog.h>
#include <boost/algorithm/string.hpp>
#include <boost/thread/tss.hpp>
#include <memory>
#include <thread>
#include <type_traits>

namespace dev
{
namespace blockverifier
{
class ExecutiveContext;
}
namespace storage
{
class ChangeLog : public std::vector<Change>
{
public:
    ChangeLog() { std::cout << std::this_thread::get_id() << " change log allocated" << std::endl; }

    ~ChangeLog() { std::cout << std::this_thread::get_id() << " change log freed" << std::endl; }
};

class MemoryTableFactory : public StateDBFactory
{
public:
    typedef std::shared_ptr<MemoryTableFactory> Ptr;
    MemoryTableFactory(bool _needRollback = false);
    virtual ~MemoryTableFactory() {}
    virtual Table::Ptr openTable(
        const std::string& tableName, bool authorityFlag = true, bool isPara = true);
    virtual Table::Ptr createTable(const std::string& tableName, const std::string& keyField,
        const std::string& valueField, bool authorityFlag = true,
        Address const& _origin = Address(), bool isPara = true);

    virtual Storage::Ptr stateStorage() { return m_stateStorage; }
    virtual void setStateStorage(Storage::Ptr stateStorage) { m_stateStorage = stateStorage; }

    void setBlockHash(h256 blockHash);
    void setBlockNum(int64_t blockNum);

    h256 hash();
    size_t savepoint();
    void rollback(size_t _savepoint);
    void commit();
    void commitDB(h256 const& _blockHash, int64_t _blockNumber);
    int getCreateTableCode() { return createTableCode; }

private:
    storage::TableInfo::Ptr getSysTableInfo(const std::string& tableName);
    void setAuthorizedAddress(storage::TableInfo::Ptr _tableInfo);
    std::vector<Change>& getChangeLog();
    Storage::Ptr m_stateStorage;
    h256 m_blockHash;
    int m_blockNum;
    // this map can't be changed, hash() need ordered data
    std::map<std::string, Table::Ptr> m_name2Table;
    // boost::thread_specific_ptr<std::vector<Change>> m_changeLog;
    thread_local static ChangeLog s_changeLog;
    h256 m_hash;
    std::vector<std::string> m_sysTables;
    int createTableCode;
    bool m_needRollback;

    // mutex
    mutable RecursiveMutex x_name2Table;
};

}  // namespace storage

}  // namespace dev
