/*
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdexcept>
#include "CSGNode.h"

using namespace std;

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))


namespace csg {

//-----------------------------------------------------------------------------
// CSGCompositeNode
//-----------------------------------------------------------------------------

CSGCompositeNode::~CSGCompositeNode()
{
  // Recursively destroy all children
  list<CSGNode *>::iterator i;
  for(i = mChildren.begin(); i != mChildren.end(); ++ i)
    delete (CSGNode *) (*i);
}

void CSGCompositeNode::SetSampleSpace(SampleSpace * aSampleSpace)
{
  // Recursively set the sample space for all children
  list<CSGNode *>::iterator i;
  for(i = mChildren.begin(); i != mChildren.end(); ++ i)
    (*i)->SetSampleSpace(aSampleSpace);

  // Allocate memory for intermediate slice
  mTmpSlice.resize(aSampleSpace->mDiv[0] * aSampleSpace->mDiv[1]);
}


//-----------------------------------------------------------------------------
// CSGUnion
//-----------------------------------------------------------------------------

void CSGUnion::GetBoundingBox(BoundingBox &aAABB)
{
  if(mChildren.empty())
  {
    aAABB.mMin = aAABB.mMax = Vector3(0.0, 0.0, 0.0);
    return;
  }

  // Get the combined bounding box for all children
  bool first = true;
  list<CSGNode *>::iterator i;
  for(i = mChildren.begin(); i != mChildren.end(); ++ i)
  {
    // Get child bounding box
    BoundingBox tmpAABB;
    (*i)->GetBoundingBox(tmpAABB);

    // Combine...
    if(first)
      aAABB = tmpAABB;
    else
      aAABB.Union(tmpAABB);

    first = false;
  }
}

void CSGUnion::ComposeSlice(Voxel * aSlice, int aZ)
{
  bool first = true;
  list<CSGNode *>::iterator i;
  for(i = mChildren.begin(); i != mChildren.end(); ++ i)
  {
    if(first)
      (*i)->ComposeSlice(aSlice, aZ);
    else
    {
      (*i)->ComposeSlice(&mTmpSlice[0], aZ);
      for(unsigned int k = 0; k < mTmpSlice.size(); ++ k)
        aSlice[k] = MAX(aSlice[k], mTmpSlice[k]);
    }
    first = false;
  }
}


//-----------------------------------------------------------------------------
// CSGIntersection
//-----------------------------------------------------------------------------

void CSGIntersection::GetBoundingBox(BoundingBox &aAABB)
{
  if(mChildren.empty())
  {
    aAABB.mMin = aAABB.mMax = Vector3(0.0, 0.0, 0.0);
    return;
  }

  // Get the combined bounding box for all children
  bool first = true;
  list<CSGNode *>::iterator i;
  for(i = mChildren.begin(); i != mChildren.end(); ++ i)
  {
    // Get child bounding box
    BoundingBox tmpAABB;
    (*i)->GetBoundingBox(tmpAABB);

    // Combine...
    if(first)
      aAABB = tmpAABB;
    else
      aAABB.Intersection(tmpAABB);

    first = false;
  }
}

void CSGIntersection::ComposeSlice(Voxel * aSlice, int aZ)
{
  bool first = true;
  list<CSGNode *>::iterator i;
  for(i = mChildren.begin(); i != mChildren.end(); ++ i)
  {
    if(first)
      (*i)->ComposeSlice(aSlice, aZ);
    else
    {
      (*i)->ComposeSlice(&mTmpSlice[0], aZ);
      for(unsigned int k = 0; k < mTmpSlice.size(); ++ k)
        aSlice[k] = MIN(aSlice[k], mTmpSlice[k]);
    }
    first = false;
  }
}


//-----------------------------------------------------------------------------
// CSGDifference
//-----------------------------------------------------------------------------

void CSGDifference::GetBoundingBox(BoundingBox &aAABB)
{
  if(mChildren.empty())
  {
    aAABB.mMin = aAABB.mMax = Vector3(0.0, 0.0, 0.0);
    return;
  }

  // For difference objects, the AABB is that of the first object
  (*mChildren.begin())->GetBoundingBox(aAABB);
}

void CSGDifference::ComposeSlice(Voxel * aSlice, int aZ)
{
  bool first = true;
  list<CSGNode *>::iterator i;
  for(i = mChildren.begin(); i != mChildren.end(); ++ i)
  {
    if(first)
      (*i)->ComposeSlice(aSlice, aZ);
    else
    {
      (*i)->ComposeSlice(&mTmpSlice[0], aZ);
      for(unsigned int k = 0; k < mTmpSlice.size(); ++ k)
        aSlice[k] = MAX(aSlice[k], -mTmpSlice[k]);
    }
    first = false;
  }
}


//-----------------------------------------------------------------------------
// CSGShape
//-----------------------------------------------------------------------------

CSGShape::~CSGShape()
{
  if(mVoxelize)
    delete mVoxelize;
}

void CSGShape::SetShape(Voxelize * aVoxelize)
{
  if(mVoxelize)
    delete mVoxelize;
  mVoxelize = aVoxelize;
}

void CSGShape::SetSampleSpace(SampleSpace * aSampleSpace)
{
  if(!mVoxelize)
    throw runtime_error("No shape defined.");

  mVoxelize->SetSampleSpace(aSampleSpace);
}

void CSGShape::GetBoundingBox(BoundingBox &aAABB)
{
  if(!mVoxelize)
    throw runtime_error("No shape defined.");

  mVoxelize->GetBoundingBox(aAABB);
}

void CSGShape::ComposeSlice(Voxel * aSlice, int aZ)
{
  if(!mVoxelize)
    throw runtime_error("No shape defined.");

  // Calculate the slice
  mVoxelize->CalculateSlice(aSlice, aZ);
}

}