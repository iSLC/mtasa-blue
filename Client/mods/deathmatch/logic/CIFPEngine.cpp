#include <StdInc.h>

std::shared_ptr < CClientIFP > CIFPEngine::EngineLoadIFP ( CResource * pResource, CClientManager * pManager, const SString & strPath, const SString & strBlockName )
{
    // Grab the resource root entity
    CClientEntity* pRoot = pResource->GetResourceIFPRoot ( );
                    
    // Check whether the IFP blockname exists or not
    if ( g_pClientGame->GetIFPPointerFromMap ( strBlockName ) == nullptr )
    { 
        // Create a IFP element
        std::shared_ptr < CClientIFP > pIFP ( new CClientIFP ( pManager, INVALID_ELEMENT_ID ) );

        // Try to load the IFP file
        if ( pIFP->LoadIFP ( strPath, strBlockName ) )
        {
            // We can use the map to retrieve correct IFP by block name later
            g_pClientGame->InsertIFPPointerToMap ( strBlockName, pIFP );

            // Success loading the file. Set parent to IFP root
            pIFP->SetParent ( pRoot );
            return pIFP;
        }
    }
    return nullptr;
}

bool CIFPEngine::EngineReplaceAnimation ( CClientEntity * pEntity, const SString & strInternalBlockName, const SString & strInternalAnimName, const SString & strCustomBlockName, const SString & strCustomAnimName )
{
    if ( IS_PED ( pEntity ) )
    { 
        CClientPed& Ped = static_cast < CClientPed& > ( *pEntity );

        CAnimBlock * pInternalBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( strInternalBlockName );
        std::shared_ptr < CClientIFP > pCustomIFP = g_pClientGame->GetIFPPointerFromMap ( strCustomBlockName );
        if ( pInternalBlock && pCustomIFP )
        {
            // Try to load the block, if it's not loaded already
            pInternalBlock->Request ( BLOCKING, true );
 
            CAnimBlendHierarchy * pInternalAnimHierarchy = g_pGame->GetAnimManager ()->GetAnimation ( strInternalAnimName, pInternalBlock );
            CAnimBlendHierarchySAInterface * pCustomAnimHierarchyInterface = pCustomIFP->GetAnimationHierarchy ( strCustomAnimName );
            if ( pInternalAnimHierarchy && pCustomAnimHierarchyInterface )
            { 
                Ped.ReplaceAnimation ( pInternalAnimHierarchy, pCustomIFP, pCustomAnimHierarchyInterface );
                return true;
            }    
        }
    }
    return false;
}

bool CIFPEngine::EngineRestoreAnimation ( CClientEntity * pEntity, const SString & strInternalBlockName, const SString & strInternalAnimName, eRestoreAnimation eRestoreType )
{
    if ( IS_PED ( pEntity ) )
    { 
        CClientPed& Ped = static_cast < CClientPed& > ( *pEntity );
        
        if ( eRestoreType == eRestoreAnimation::ALL )
        {
            Ped.RestoreAllAnimations ( );
            return true;
        }
        else
        { 
            CAnimBlock * pInternalBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( strInternalBlockName );
            if ( pInternalBlock )
            {
                if ( eRestoreType == eRestoreAnimation::BLOCK )
                {
                    Ped.RestoreAnimations ( *pInternalBlock );
                    return true;
                }
                else 
                { 
                    CAnimBlendHierarchy * pInternalAnimHierarchy = g_pGame->GetAnimManager ()->GetAnimation ( strInternalAnimName, pInternalBlock );
                    if ( pInternalAnimHierarchy )
                    { 
                        Ped.RestoreAnimation ( pInternalAnimHierarchy );
                        return true;
                    }    
                }
            }
        }
    }
    return false;
}
