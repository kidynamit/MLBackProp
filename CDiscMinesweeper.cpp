#include "CDiscMinesweeper.h"

//-----------------------------------constructor-------------------------
//
//-----------------------------------------------------------------------
CDiscMinesweeper::CDiscMinesweeper():
							 CMinesweeper(),
                             m_dRotation((ROTATION_DIRECTION)RandInt(0,3))
{
	//create a random start position
	
	m_vPosition = SVector2D<int>(RandInt(0,CParams::WindowWidth/CParams::iGridCellDim)*CParams::iGridCellDim, 
					             RandInt(0,CParams::WindowHeight/CParams::iGridCellDim)*CParams::iGridCellDim);
}

//-------------------------------------------Reset()--------------------
//
//	Resets the sweepers position, MinesGathered and rotation
//
//----------------------------------------------------------------------
void CDiscMinesweeper::Reset()
{

	//reset the sweepers positions
	m_vPosition = SVector2D<int>(RandInt(0,CParams::WindowWidth/CParams::iGridCellDim)*CParams::iGridCellDim, 
					             RandInt(0,CParams::WindowHeight/CParams::iGridCellDim)*CParams::iGridCellDim);
	
	CMinesweeper::Reset();

	//and the rotation
	m_dRotation = (ROTATION_DIRECTION)RandInt(0,3);
	//m_dRotation = ROTATION_DIRECTION::SOUTH;
	return;
}

//---------------------WorldTransform--------------------------------
//
//	sets up a translation matrix for the sweeper according to its
//  scale, rotation and position. Returns the transformed vertices.
//-------------------------------------------------------------------
void CDiscMinesweeper::WorldTransform(vector<SPoint> &sweeper)
{
	//create the world transformation matrix
	C2DMatrix matTransform;
	
	//scale
	matTransform.Scale(m_dScale, m_dScale);
	
	//rotate
	matTransform.Rotate((m_dRotation-1) * CParams::dHalfPi);
	
	//and translate
	matTransform.Translate(m_vPosition.x, m_vPosition.y);
	
	//now transform the ships vertices
	matTransform.TransformSPoints(sweeper);
}
#include <assert.h>
//-------------------------------Update()--------------------------------
//
//	First we take sensor readings. These are then fed into the learning algorithm
//
//	The inputs are:
//	
//	A vector to the closest mine (x, y)
//	The sweepers 'look at' vector (x, y)
//	So given a force we calculate the resultant rotation 
//	and acceleration. This is then applied to current velocity vector.
//
//-----------------------------------------------------------------------
bool CDiscMinesweeper::Update(vector<CDiscCollisionObject*> &objects)
{
	GetClosestObjects(objects);
	//update Look At 
	m_vLookAt.x = cos(m_dRotation * CParams::dHalfPi);
	m_vLookAt.y = sin(m_dRotation * CParams::dHalfPi);

	//update position
	m_vPrevPosition = m_vPosition;
	m_vPosition += (m_vLookAt * CParams::iGridCellDim);

	//wrap around window limits
	if (m_vPosition.x >= CParams::WindowWidth) m_vPosition.x = 0;
	if (m_vPosition.x < 0) m_vPosition.x = CParams::WindowWidth - CParams::iGridCellDim;
	if (m_vPosition.y >= CParams::WindowHeight) m_vPosition.y = 0;
	if (m_vPosition.y < 0) m_vPosition.y = CParams::WindowHeight - CParams::iGridCellDim;
	return true;
}


//----------------------GetClosestObject()---------------------------------
//
//	returns the vector from the sweeper to the closest mine
//
//-----------------------------------------------------------------------
void CDiscMinesweeper::GetClosestObjects(vector<CDiscCollisionObject*> &objects)
{
	return;
}
//----------------------------- CheckForObject -----------------------------
//
//  this function checks for collision with its closest mine (calculated
//  earlier and stored in m_iClosestMine)
//-----------------------------------------------------------------------
int CDiscMinesweeper::CheckForObject(vector<CDiscCollisionObject*> &objects, int size)
{
	std::vector<CDiscCollisionObject *>::iterator collision = std::find_if(objects.begin(), objects.end(),  
		[this](CDiscCollisionObject * obj)->bool 
	{ 
		return !obj->isDead() && 
			obj->getPosition().x == this->Position().x && 
			obj->getPosition().y ==  this->Position().y;
	});

	if (collision == objects.end())
		return -1;
	else
		return std::distance(objects.begin(), collision );
}
//-----------------------------------------------------------------------
// Getters and setters for rotation force and speed
//-----------------------------------------------------------------------
void CDiscMinesweeper::setRotation(ROTATION_DIRECTION dir)
{
	m_dRotation = dir;
}

ROTATION_DIRECTION CDiscMinesweeper::getRotation()
{
	return m_dRotation;
}

