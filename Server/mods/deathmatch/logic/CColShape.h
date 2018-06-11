/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColShape.h
 *  PURPOSE:     Base shaped collision entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CElement.h"

enum eColShapeType
{
    COLSHAPE_CIRCLE,
    COLSHAPE_CUBOID,
    COLSHAPE_SPHERE,
    COLSHAPE_RECTANGLE,
    COLSHAPE_POLYGON,
    COLSHAPE_TUBE,
};

class CColShape : public CElement
{
public:
    CColShape(class CColManager* pManager, CElement* pParent, CXMLNode* pNode = NULL, bool bIsPartnered = false);
    virtual ~CColShape(void);

    virtual eColShapeType GetShapeType(void) = 0;

    void Unlink(void);

    virtual bool DoHitDetection(const CVector& vecNowPosition) = 0;

    bool IsEnabled(void) { return m_bIsEnabled; };
    void SetEnabled(bool bEnabled) { m_bIsEnabled = bEnabled; };

    const CVector& GetPosition(void);
    virtual void   SetPosition(const CVector& vecPosition);

    void                CallHitCallback(CElement& Element);
    void                CallLeaveCallback(CElement& Element);
    class CColCallback* SetCallback(class CColCallback* pCallback) { return (m_pCallback = pCallback); };

    bool GetAutoCallEvent(void) { return m_bAutoCallEvent; };
    void SetAutoCallEvent(bool bAutoCallEvent) { m_bAutoCallEvent = bAutoCallEvent; };

    void                      AddCollider(CElement* pElement) { m_Colliders.push_back(pElement); }
    void                      RemoveCollider(CElement* pElement) { m_Colliders.remove(pElement); }
    bool                      ColliderExists(CElement* pElement);
    void                      RemoveAllColliders(void);
    list<CElement*>::iterator CollidersBegin(void) { return m_Colliders.begin(); }
    list<CElement*>::iterator CollidersEnd(void) { return m_Colliders.end(); }

    bool IsPartnered(void) { return m_bPartnered; }

    void SizeChanged(void);

protected:
    CVector m_vecPosition;

private:
    bool                m_bIsEnabled;
    class CColManager*  m_pManager;
    class CColCallback* m_pCallback;
    bool                m_bAutoCallEvent;

    list<CElement*> m_Colliders;

    bool m_bPartnered;
};

