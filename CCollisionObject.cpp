#include "CCollisionObject.h"


CCollisionObject::CCollisionObject(ObjectType objectType = ObjectType::Mine) : 
									m_ObjectType(objectType),
									m_bDead(false), 
									m_bTarget(false), 
									m_bAttainable(m_ObjectType == ObjectType::Mine)
{
}


CCollisionObject::~CCollisionObject(void)
{
}

void CCollisionObject::setType(ObjectType objectType)
{
	m_ObjectType = objectType;
}

CCollisionObject::ObjectType CCollisionObject::getType()
{
	return m_ObjectType;
}
void CCollisionObject::Reset()
{
	this->m_bDead = false;
	this->m_bTarget = false;
	this->m_bAttainable = true;
}
void CCollisionObject::die(){
	this->m_bDead = true;
	this->m_bTarget = false;
	this->m_bAttainable = false;
}
void CCollisionObject::setTarget(bool newTarget){
	this->m_bTarget = (!this->m_bDead && this->m_ObjectType == ObjectType::Mine) ? newTarget && this->m_bAttainable : false;
}
void CCollisionObject::setAttainable(bool newAttainableMine) {
	this->m_bAttainable = (!this->m_bDead && this->m_ObjectType == ObjectType::Mine) ? newAttainableMine : false;
	this->m_bTarget = this->m_bTarget && (this->m_bAttainable);
}

bool CCollisionObject::isDead() const{
	return this->m_bDead;
}
bool CCollisionObject::isTarget() const {
	return this->m_bTarget;
}
bool CCollisionObject::isAttainable() const {
	return this->m_bAttainable;
}