/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRegistry.h
 *  PURPOSE:     SQLite registry abstraction class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

typedef CAutoRefedPointer<struct CRegistryResultData> CRegistryResult;

#pragma once

#include "CLogger.h"
#include <list>
#include <vector>
#include <string>
#include "../../../vendor/sqlite/sqlite3.h"

// Only used for identifying 8 byte integers in varargs list
#define SQLITE_INTEGER64 10

class CRegistry
{
    friend class CRegistryManager;
    CRegistry(const std::string& strFileName);
    ~CRegistry(void);

public:
    void SuspendBatching(uint uiTicks);
    void Load(const std::string& strFileName);
    bool IntegrityCheck(void);

    void CreateTable(const std::string& strTable, const std::string& strDefinition, bool bSilent = false);
    void DropTable(const std::string& strTable);

    bool Delete(const std::string& strTable, const std::string& strWhere);
    bool Insert(const std::string& strTable, const std::string& strValues, const std::string& strColumns);
    bool Select(const std::string& strColumns, const std::string& strTable, const std::string& strWhere, unsigned int uiLimit, CRegistryResult* pResult);
    bool Update(const std::string& strTable, const std::string& strSet, const std::string& strWhere);

    bool Query(const std::string& strQuery, class CLuaArguments* pArgs, CRegistryResult* pResult);
    bool Query(const char* szQuery, ...);
    bool Query(CRegistryResult* pResult, const char* szQuery, ...);

    const SString& GetLastError(void) { return m_strLastErrorMessage; }

protected:
    bool SetLastErrorMessage(const std::string& strLastErrorMessage, const std::string& strQuery);
    bool Exec(const std::string& strQuery);
    bool ExecInternal(const char* szQuery);
    bool Query(CRegistryResult* pResult, const char* szQuery, va_list vl);
    bool QueryInternal(const char* szQuery, CRegistryResult* pResult);
    void BeginAutomaticTransaction(void);
    void EndAutomaticTransaction(void);

    sqlite3*  m_db;
    bool      m_bOpened;
    bool      m_bInAutomaticTransaction;
    long long m_llSuspendBatchingEndTime;
    SString   m_strLastErrorMessage;
    SString   m_strLastErrorQuery;
    SString   m_strFileName;

private:
    bool Query(const char* szQuery, CRegistryResult* pResult);            // Not defined to catch incorrect usage
};

struct CRegistryResultCell
{
    CRegistryResultCell(void)
    {
        nType = SQLITE_NULL;
        nLength = 0;
        pVal = NULL;
    }
    CRegistryResultCell(const CRegistryResultCell& cell)
    {
        nType = cell.nType;
        nLength = cell.nLength;
        nVal = cell.nVal;
        fVal = cell.fVal;
        pVal = NULL;
        if ((nType == SQLITE_BLOB || nType == SQLITE_TEXT) && cell.pVal && nLength > 0)
        {
            pVal = new unsigned char[nLength];
            memcpy(pVal, cell.pVal, nLength);
        }
    };
    ~CRegistryResultCell(void)
    {
        if (pVal)
            delete[] pVal;
    }

    CRegistryResultCell& operator=(const CRegistryResultCell& cell)
    {
        if (pVal)
            delete[] pVal;

        nType = cell.nType;
        nLength = cell.nLength;
        nVal = cell.nVal;
        fVal = cell.fVal;
        pVal = NULL;
        if ((nType == SQLITE_BLOB || nType == SQLITE_TEXT) && cell.pVal && nLength > 0)
        {
            pVal = new unsigned char[nLength];
            memcpy(pVal, cell.pVal, nLength);
        }
        return *this;
    }

    template <class T>
    void GetNumber(T& outValue) const
    {
        outValue = GetNumber<T>();
    }

    template <class T>
    T GetNumber(void) const
    {
        if (nType == SQLITE_INTEGER)
            return static_cast<T>(nVal);
        if (nType == SQLITE_FLOAT)
            return static_cast<T>(fVal);
        return 0;
    }

    int nType;              // Type identifier, SQLITE_*
    int nLength;            // Length in bytes if nType == SQLITE_BLOB or SQLITE_TEXT
                            //    (includes zero terminator if TEXT)
    long long int  nVal;
    float          fVal;
    unsigned char* pVal;
};

typedef std::vector<CRegistryResultCell>              CRegistryResultRow;
typedef std::list<CRegistryResultRow>::const_iterator CRegistryResultIterator;

struct CRegistryResultData
{
    CRegistryResultData(void)
    {
        nRows = 0;
        nColumns = 0;
        uiNumAffectedRows = 0;
        ullLastInsertId = 0;
        pNextResult = nullptr;
    }
    ~CRegistryResultData(void) { SAFE_DELETE(pNextResult); }
    std::vector<SString>          ColNames;
    std::list<CRegistryResultRow> Data;
    int                           nRows;
    int                           nColumns;
    uint                          uiNumAffectedRows;
    uint64                        ullLastInsertId;
    CRegistryResultData*          pNextResult;

    CRegistryResultData*    GetThis() { return this; }
    CRegistryResultIterator begin(void) const { return Data.begin(); }
    CRegistryResultIterator end(void) const { return Data.end(); }
};
