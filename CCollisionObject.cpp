#include "CCollisionObject.h"


CCollisionObject::CCollisionObject(ObjectType objectType = ObjectType::Mine) : 
									m_ObjectType(objectType),
									m_bDead(false), 
									m_bTarget(false)
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
}
void CCollisionObject::die(){
	this->m_bDead = true;
	this->m_bTarget = false;
}
void CCollisionObject::target(){
	this->m_bTarget = (!this->m_bDead && this->m_ObjectType == ObjectType::Mine) ? true : false;
}

bool CCollisionObject::isDead() const{
	return m_bDead;
}
bool CCollisionObject::isTarget() const {
	return (this->m_ObjectType == ObjectType::Mine) ? m_bTarget : false;
}