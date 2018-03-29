#include "StdInc.h"

#include <../game_sa/CAnimBlendHierarchySA.h> 
#include <../game_sa/CAnimBlendStaticAssociationSA.h>
#include <../game_sa/CAnimBlendAssociationSA.h>
#include <../game_sa/CAnimBlendAssocGroupSA.h>

DWORD FUNC_NEW_OPERATOR =                                           0x082119A;
DWORD FUNC_CAnimBlendAssociation_Constructor =                      0x04CF080;
DWORD FUNC_CAnimBlendAssociation__ReferenceAnimBlock =              0x4CEA50;
DWORD FUNC_UncompressAnimation =                                    0x4D41C0;
DWORD FUNC_CAnimBlendAssociation__CAnimBlendAssociation_hierarchy = 0x4CEFC0;

DWORD RETURN_CAnimBlendAssoc_destructor =                   0x4CECF6;
DWORD RETURN_CAnimBlendAssocGroup_CopyAnimation_NORMALFLOW =0x4CE151; 
DWORD RETURN_CAnimBlendAssocGroup_CopyAnimation =           0x4CE187; 
DWORD RETURN_CAnimBlendAssocGroup_CopyAnimation_ERROR =     0x4CE199;
DWORD RETURN_CAnimManager_AddAnimation =                    0x4D3AB1;
DWORD RETURN_CAnimManager_AddAnimationAndSync =             0x4D3B41;
DWORD RETURN_CAnimManager_BlendAnimation_Hierarchy =        0x4D4577; 

CAnimBlendAssocDestructorHandler * m_pCAnimBlendAssocDestructorHandler = nullptr;
AddAnimationHandler * m_pAddAnimationHandler = nullptr;
AddAnimationAndSyncHandler * m_pAddAnimationAndSyncHandler = nullptr;
AssocGroupCopyAnimationHandler * m_pAssocGroupCopyAnimationHandler = nullptr;
BlendAnimationHierarchyHandler * m_pBlendAnimationHierarchyHandler = nullptr;

int _cdecl OnCAnimBlendAssocGroupCopyAnimation ( AssocGroupId animGroup, int iAnimId );
auto CAnimBlendStaticAssociation_FreeSequenceArray = (hCAnimBlendStaticAssociation_FreeSequenceArray)0x4ce9a0;

void CMultiplayerSA::SetCAnimBlendAssocDestructorHandler ( CAnimBlendAssocDestructorHandler * pHandler )
{
    m_pCAnimBlendAssocDestructorHandler = pHandler;
}

void CMultiplayerSA::SetAddAnimationHandler ( AddAnimationHandler * pHandler )
{
    m_pAddAnimationHandler = pHandler;
}

void CMultiplayerSA::SetAddAnimationAndSyncHandler ( AddAnimationAndSyncHandler * pHandler )
{
    m_pAddAnimationAndSyncHandler = pHandler;
}

void CMultiplayerSA::SetAssocGroupCopyAnimationHandler ( AssocGroupCopyAnimationHandler * pHandler )
{
    m_pAssocGroupCopyAnimationHandler = pHandler;
}

void CMultiplayerSA::SetBlendAnimationHierarchyHandler ( BlendAnimationHierarchyHandler * pHandler )
{
    m_pBlendAnimationHierarchyHandler = pHandler;
}

void CAnimBlendAssoc_destructor ( CAnimBlendAssociationSAInterface * pThis )
{
    if ( m_pCAnimBlendAssocDestructorHandler )
    {
        m_pCAnimBlendAssocDestructorHandler ( pThis );
    }
}

void _declspec(naked) HOOK_CAnimBlendAssoc_destructor ()
{
    _asm
    {
        push    ecx  // this
        call    CAnimBlendAssoc_destructor
        pop     ecx
        push    esi
        mov     esi, ecx
        mov     eax, [esi+10h]
        jmp     RETURN_CAnimBlendAssoc_destructor
    }
}

CAnimBlendStaticAssociationSAInterface * __cdecl AllocateStaticAssociationMemory ( void )
{
    return new CAnimBlendStaticAssociationSAInterface;
}

void __cdecl DeleteStaticAssociation ( CAnimBlendStaticAssociationSAInterface * pAnimStaticAssoc )
{
    CAnimBlendStaticAssociation_FreeSequenceArray ( pAnimStaticAssoc );
    delete pAnimStaticAssoc;
}

void _declspec(naked) HOOK_CAnimBlendAssocGroup_CopyAnimation ()
{
    _asm
    {
        pushad
    }

    if ( m_pAssocGroupCopyAnimationHandler )
    {
        _asm
        {
            popad                        
 
            push    ecx
            push    ebp   
            mov     ebp, esp
            sub esp, 4

            push    eax
            push    ecx
            push    edi
            
            // create CAnimBlendAssociation
            push    3Ch   
            call    FUNC_NEW_OPERATOR
            add     esp, 4

            mov     [ebp-4], eax
            push    eax 

            // Allocate memory for our new static association
            call    AllocateStaticAssociationMemory
            mov     edi, eax

            // push the static association
            push    edi
            call    m_pAssocGroupCopyAnimationHandler //CAnimBlendAssocGroup_CopyAnimation
            add     esp, 14h
            
            mov     ecx, [ebp-4] 

            add     esp, 4 // remove space for local var
            mov     esp, ebp
            pop     ebp

            // save eax and ecx for later to check whether current animation is custom or not 
            // after calling FUNC_CAnimBlendAssociation_Constructor function
            push    eax // isCustomAnimation ( bool )
            push    ecx // pIFPAnimations

            // get "this" from stack that we pushed first
            mov     ecx, [esp+8]
            mov     ecx, [ecx+4]
            sub     eax, edx
            push    esi

            // copy the static association to esi
            mov     esi, edi 
            test    esi, esi
            jz      ERROR_CopyAnimation
            mov     eax, [esi+10h]
            push    eax
            mov     eax, 04D41C0h
            call    eax
            add     esp, 4
            mov     eax, [esp+4] // pAnimAssociation
            mov     [esp+20h], eax 
            test    eax, eax
            mov     [esp+18h], 0   
            jz      ERROR_CopyAnimation
            push    esi
            mov     ecx, eax
            call    FUNC_CAnimBlendAssociation_Constructor
            mov     edi, eax

            // Delete our static association, since we no longer need it 
            push    esi
            call    DeleteStaticAssociation
            add     esp, 4 
            
            mov     ecx, [esp+4] // pIFPAnimations
            mov     eax, [esp+8] // isCustomAnimation
            
            // put CAnimBlendAssociation in eax
            mov     eax, edi
            add     esp, 0Ch
            jmp     RETURN_CAnimBlendAssocGroup_CopyAnimation

            ERROR_CopyAnimation:
            add     esp, 0Ch
            // Delete our static association
            push    edi
            call    DeleteStaticAssociation
            add     esp, 4
            jmp    RETURN_CAnimBlendAssocGroup_CopyAnimation_ERROR
        }
    }

    _asm
    {
        popad
        mov     ecx, [ecx+4]
        sub     eax, edx
        jmp RETURN_CAnimBlendAssocGroup_CopyAnimation_NORMALFLOW
    }
}

void _declspec(naked) HOOK_CAnimManager_AddAnimation ()
{
    _asm
    {
        pushad
    }

    if ( m_pAddAnimationHandler )
    {
        _asm
        {    
            popad
            mov     ecx, [esp+4]  // animationClump
            mov     edx, [esp+8]  // animationGroup
            mov     eax, [esp+12] // animationID
            push    eax
            push    edx
            call    OnCAnimBlendAssocGroupCopyAnimation
            add     esp, 8
            mov     [esp+12], eax // replace animationID
            
            // call our handler function
            push    eax 
            push    edx
            mov     ecx, [esp+12] // animationClump
            push    ecx
            call    m_pAddAnimationHandler
            add     esp, 0Ch
            pushad
            jmp     NORMAL_FLOW_AddAnimation
        }
    }

    _asm
    {
        NORMAL_FLOW_AddAnimation:
        popad
        mov     eax, dword ptr [esp+0Ch] 
        mov     edx, dword ptr ds:[0B4EA34h] 
        push    esi
        push    edi
        push    eax
        mov     eax, dword ptr [esp+14h] // animationGroup
        mov     edi, dword ptr [esp+10h] // animationClump
        jmp     RETURN_CAnimManager_AddAnimation
    } 
}

void _declspec(naked) HOOK_CAnimManager_AddAnimationAndSync ()
{
    _asm
    {
        pushad
    }

    if ( m_pAddAnimationAndSyncHandler )
    {
         _asm
        {    
            popad
            mov     ecx, [esp+4]  // animationClump
            mov     ebx, [esp+8]  // pAnimAssociationToSyncWith
            mov     edx, [esp+12] // animationGroup
            mov     eax, [esp+16] // animationID
            push    eax
            push    edx
            call    OnCAnimBlendAssocGroupCopyAnimation
            add     esp, 8
            mov     [esp+16], eax // replace animationID
            
            // call our handler function
            push    eax
            push    edx
            push    ebx
            mov     ecx, [esp+16] // animationClump
            push    ecx
            call    m_pAddAnimationAndSyncHandler
            add     esp, 10h
            pushad
            jmp     NORMAL_FLOW_AddAnimationAndSync
        }
    }

    _asm
    {
        NORMAL_FLOW_AddAnimationAndSync:
        popad
        mov     eax, dword ptr [esp+10h]
        mov     edx, dword ptr ds:[0B4EA34h] 
        push    esi
        push    edi
        push    eax
        mov     eax, dword ptr [esp+18h] // animationGroup
        mov     edi, dword ptr [esp+10h] // animationClump
        jmp     RETURN_CAnimManager_AddAnimationAndSync
    }
}

void _declspec(naked) HOOK_CAnimManager_BlendAnimation_Hierarchy () 
{
    _asm
    {
        pushad
    }

    if ( m_pBlendAnimationHierarchyHandler  )
    {
        _asm
        {        
            popad
            push    eax // pAnimAssociation
            push    ecx // pAnimHierarchy

            push    ebp   
            mov     ebp, esp
            sub     esp, 8
            
            // call our handler function
            mov     edx, [ebp+28h+4+12] 
            push    edx  // pClump
            lea     edx, [ebp-4]
            push    edx  //  CAnimBlendHierarchySAInterface ** pOutAnimHierarchy
            mov     edx, [esp+24]
            push    edx  // pAnimAssociation
            call    m_pBlendAnimationHierarchyHandler //CAnimManager_BlendAnimation_Hierarchy
            add     esp, 0Ch
 
            mov     ecx, [ebp-4] // pCustomAnimHierarchy

            add     esp, 8 // remove space for local var
            mov     esp, ebp
            pop     ebp
          
             // Check if it's a custom animation or not
            cmp     al, 00
            je      NOT_CUSTOM_ANIMATION_CAnimManager_BlendAnimation_Hierarchy
            
            // Replace animation hierarchy with our custom one
            mov     [esp], ecx 
            mov     [esp+28h+8+8], ecx

            NOT_CUSTOM_ANIMATION_CAnimManager_BlendAnimation_Hierarchy:
            pop ecx
            pop eax
            pushad
            jmp NORMAL_FLOW_BlendAnimation_Hierarchy
        } 
    }

    _asm
    {
        NORMAL_FLOW_BlendAnimation_Hierarchy:
        popad
        mov     edx, [esp+28h+4] 
        push    ecx      // pAnimHierarchy
        push    edx      // pClump
        mov     ecx, eax // pAnimAssociation
        call    FUNC_CAnimBlendAssociation__CAnimBlendAssociation_hierarchy
        mov     esi, eax
        mov     ax, word ptr [esp+28h+0Ch] // flags
        mov     ecx, esi
        mov     [esp+28h-4], 0FFFFFFFFh // var_4
        mov     [esp+28h-18h], esi
        mov     [esi+2Eh], ax
        call    FUNC_CAnimBlendAssociation__ReferenceAnimBlock
        mov     ecx, [esp+28h+8] // pAnimHierarchy
        push    ecx
        call    FUNC_UncompressAnimation
        jmp    RETURN_CAnimManager_BlendAnimation_Hierarchy
    }

}
