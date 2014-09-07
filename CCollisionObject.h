#pragma once
class CCollisionObject
{
public:
	enum ObjectType { Mine, SuperMine, Rock };
protected:
	ObjectType	m_ObjectType;
	bool		m_bDead;
	bool		m_bTarget;
	bool		m_bAttainable;

public:
	CCollisionObject(ObjectType objectType);
	virtual ~CCollisionObject(void);
	void setType(ObjectType objectType);
	ObjectType getType();
	void Reset();
	void die();
	void setTarget(bool newTarget=true);
	void setAttainable(bool newAttainableMine=false);
	bool isDead() const;
	bool isTarget() const;
	bool isAttainable() const;
};