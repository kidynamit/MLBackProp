#pragma once
class CCollisionObject
{
public:
	enum ObjectType { Mine, SuperMine, Rock };
protected:
	ObjectType	m_ObjectType;
	bool		m_bDead;
	bool		m_bTarget;
	double		m_dTarget;
	double		m_dPredator = 9999;
public:
	CCollisionObject(ObjectType objectType);
	virtual ~CCollisionObject(void);
	void setType(ObjectType objectType);
	ObjectType getType();
	void Reset();
	void die();
	bool isDead() const;
	bool isTarget() const;
	void setTarget(bool targetStatus);
	void updateDistance(double distancePredator);
	double getDistance() const { return (m_ObjectType == ObjectType::Mine) ? m_dPredator : 99999; }
};

