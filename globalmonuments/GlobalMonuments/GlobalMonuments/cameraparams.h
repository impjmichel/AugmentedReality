#pragma once

#include "Stdafx.h"

CvMat* intrinsic = (CvMat*)cvLoad("intrinsic.xml");	
CvMat* distor = (CvMat*)cvLoad("distortion.xml");

Mat cameraMatrix = cvarrToMat(intrinsic);
Mat distortions = cvarrToMat(distor);



