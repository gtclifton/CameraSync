//=============================================================================
// Copyright © 2017 FLIR Integrated Imaging Solutions, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

/**
*  @example Trigger.cpp
*
*  @brief Trigger.cpp shows how to trigger the camera. It relies on information
*	provided in the Enumeration, Acquisition, and NodeMapInfo examples.
*
*	It can also be helpful to familiarize yourself with the ImageFormatControl
*	and Exposure examples. As they are somewhat shorter and simpler, either
*	provides a strong introduction to camera customization.
*
*	This example shows the process of configuring, using, and cleaning up a
*	camera for use with both a software and a hardware trigger.
*/

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

// Use the following enum and global constant to select whether a software or
// hardware trigger is used.
enum triggerType
{
	SOFTWARE,
	HARDWARE
};

const triggerType chosenTrigger = SOFTWARE;

// This function configures the PRIMARY CAMERA. First the trigger mode
// is turned off, then the LineSelector is switched to Line2 and 3.3V 
// enabled. The Trigger mode remains off.
int ConfigureTrigger(INodeMap & nodeMap)
{
	int result = 0;

	cout << endl << endl << "*** CONFIGURING TRIGGER ***" << endl << endl;

	if (chosenTrigger == SOFTWARE)
	{
		cout << "Software trigger chosen..." << endl;
	}
	else if (chosenTrigger == HARDWARE)
	{
		cout << "Hardware trigger chosen..." << endl;
	}

	try
	{
		//
		// Ensure trigger mode off
		//
		// *** NOTES ***
		// The trigger must be disabled in order to configure whether the source
		// is software or hardware.
		//
		CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
		if (!IsAvailable(ptrTriggerMode) || !IsReadable(ptrTriggerMode))
		{
			cout << "Unable to disable trigger mode (node retrieval). Aborting..." << endl;
			return -1;
		}

		CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
		if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
		{
			cout << "Unable to disable trigger mode (enum entry retrieval). Aborting..." << endl;
			return -1;
		}

		ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());

		cout << "Trigger mode disabled..." << endl;



		// Find device serial number
		string deviceSerialNumber = "";
		CStringPtr ptrStringSerial = nodeMap.GetNode("DeviceSerialNumber");
		if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
		{
			deviceSerialNumber = ptrStringSerial->GetValue();
		}
		if (deviceSerialNumber == "16276718")
		{
			//
			// Select trigger source
			//
			// *** NOTES ***
			// The trigger source must be set to hardware or software while trigger 
			// mode is off.
			//
			CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");
			if (!IsAvailable(ptrTriggerSource) || !IsWritable(ptrTriggerSource))
			{
				cout << "Unable to set trigger mode (node retrieval). Aborting..." << endl;
				return -1;
			}

			if (chosenTrigger == SOFTWARE)
			{
				// Set trigger mode to software
				CEnumEntryPtr ptrTriggerSourceSoftware = ptrTriggerSource->GetEntryByName("Software");
				if (!IsAvailable(ptrTriggerSourceSoftware) || !IsReadable(ptrTriggerSourceSoftware))
				{
					cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
					return -1;
				}

				ptrTriggerSource->SetIntValue(ptrTriggerSourceSoftware->GetValue());

				cout << "Trigger source set to software..." << ptrTriggerSourceSoftware->GetValue() << endl;
			}
			else if (chosenTrigger == HARDWARE)
			{
				// Set trigger mode to hardware ('Line0')
				CEnumEntryPtr ptrTriggerSourceHardware = ptrTriggerSource->GetEntryByName("Line0");
				if (!IsAvailable(ptrTriggerSourceHardware) || !IsReadable(ptrTriggerSourceHardware))
				{
					cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
					return -1;
				}
				ptrTriggerSource->SetIntValue(ptrTriggerSourceHardware->GetValue());
				cout << "Trigger source set to hardware..." << endl;
			}

			//
			//
			// Is it possible to read/write to "LineSelector"?
			//
			CEnumerationPtr ptrLineSelector = nodeMap.GetNode("LineSelector");
			if (!IsAvailable(ptrLineSelector) || !IsReadable(ptrLineSelector))
			{
				cout << "Unable to edit Line Selector (enum entry retrieval). Aborting..." << endl;
				return -1;
			}
			// Set LineSelector to Line2
			CEnumEntryPtr ptrLineSelector2 = ptrLineSelector->GetEntryByName("Line2");
			if (!IsAvailable(ptrLineSelector2) || !IsReadable(ptrLineSelector2))
			{
				cout << "Unable to set line selector (enum entry retrieval). Aborting..." << endl;
				return -1;
			}
			ptrLineSelector->SetIntValue(ptrLineSelector2->GetValue());
			cout << "Line Selector set to Line2..." << endl;

			// TODO: Blackfly and Flea3 GEV cameras need 1 second delay after trigger mode is turned on 

			//
			// Enable 3.3V
			//
			CBooleanPtr ptr33V = nodeMap.GetNode("V3_3Enable");
			if (!IsAvailable(ptr33V) || !IsReadable(ptr33V))
			{
				cout << "Unable to edit 3.3V Enabler (bool entry retrieval). Aborting..." << endl;
				return -1;
			}
			ptr33V->SetValue(1);
			cout << "Enabled 3.3V" << endl;


			////
			//// Turn trigger mode on
			//CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
			//if (!IsAvailable(ptrTriggerModeOn) || !IsReadable(ptrTriggerModeOn))
			//{
			//	cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << endl;
			//	return -1;
			//}
			//ptrTriggerMode->SetIntValue(ptrTriggerModeOn->GetValue());
			//// TODO: Blackfly and Flea3 GEV cameras need 1 second delay after trigger mode is turned on 
			//cout << "Trigger mode turned back on..." << endl << endl;

			cout << "Trigger mode remains off..." << endl << endl;
		}
		else
		{
			//
			// Select trigger source
			//
			// *** NOTES ***
			// The trigger source must be set to hardware or software while trigger 
			// mode is off.
			//
			CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");
			if (!IsAvailable(ptrTriggerSource) || !IsWritable(ptrTriggerSource))
			{
				cout << "Unable to set trigger mode (node retrieval). Aborting..." << endl;
				return -1;
			}

			if (chosenTrigger == SOFTWARE)
			{
				// Set trigger mode to software
				CEnumEntryPtr ptrTriggerSourceSoftware = ptrTriggerSource->GetEntryByName("Line3");
				if (!IsAvailable(ptrTriggerSourceSoftware) || !IsReadable(ptrTriggerSourceSoftware))
				{
					cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
					return -1;
				}

				ptrTriggerSource->SetIntValue(ptrTriggerSourceSoftware->GetValue());

				cout << "Trigger source set to software..." << endl;
			}
			else if (chosenTrigger == HARDWARE)
			{
				// Set trigger mode to hardware ('Line0')
				CEnumEntryPtr ptrTriggerSourceHardware = ptrTriggerSource->GetEntryByName("Line0");
				if (!IsAvailable(ptrTriggerSourceHardware) || !IsReadable(ptrTriggerSourceHardware))
				{
					cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
					return -1;
				}
				ptrTriggerSource->SetIntValue(ptrTriggerSourceHardware->GetValue());
				cout << "Trigger source set to hardware..." << endl;
			}


			//
			// Set Trigger Overlap
			CEnumerationPtr ptrTriggerOverlap = nodeMap.GetNode("TriggerOverlap");
			if (!IsAvailable(ptrTriggerOverlap) || !IsReadable(ptrTriggerOverlap))
			{
				cout << "Unable to find trigger overlap (node retrieval). Aborting..." << endl;
				return -1;
			}
			// Set trigger overlap to "Readout"
			CEnumEntryPtr ptrTriggerOverlapRO = ptrTriggerOverlap->GetEntryByName("ReadOut");
			if (!IsAvailable(ptrTriggerOverlapRO) || !IsReadable(ptrTriggerOverlapRO))
			{
				cout << "Unable to set trigger overlap (enum entry retrieval). Aborting..." << endl;
				return -1;
			}
			ptrTriggerOverlap->SetIntValue(ptrTriggerOverlapRO->GetValue());
			cout << "Trigger overlap set to Read Out..." << endl;

			//
			// Set Exposure
			CFloatPtr ptrExposureTime = nodeMap.GetNode("ExposureTime");
			if (!IsAvailable(ptrExposureTime) || !IsWritable(ptrExposureTime))
			{
				cout << "Unable to set exposure time. Aborting..." << endl << endl;
				return -1;
			}
			// Ensure desired exposure time does not exceed the maximum
			const double exposureTimeMax = ptrExposureTime->GetMax();
			double exposureTimeToSet = 4000.0; // in microseconds

			if (exposureTimeToSet > exposureTimeMax)
			{
				exposureTimeToSet = exposureTimeMax;
			}
			ptrExposureTime->SetValue(exposureTimeToSet);
			cout << "Exposure time set to " << exposureTimeToSet << " us..." << endl << endl;



			//
			// Turn trigger mode on
			//
			// *** LATER ***
			// Once the appropriate trigger source has been set, turn trigger mode 
			// on in order to retrieve images using the trigger.
			//

			CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
			if (!IsAvailable(ptrTriggerModeOn) || !IsReadable(ptrTriggerModeOn))
			{
				cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << endl;
				return -1;
			}

			ptrTriggerMode->SetIntValue(ptrTriggerModeOn->GetValue());

			// TODO: Blackfly and Flea3 GEV cameras need 1 second delay after trigger mode is turned on 

			cout << "Trigger mode turned back on..." << endl << endl;
		}

	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}
	return result;



}


// This function retrieves a single image using the trigger. In this example, 
// only a single image is captured and made available for acquisition - as such,
// attempting to acquire two images for a single trigger execution would cause 
// the example to hang. This is different from other examples, whereby a 
// constant stream of images are being captured and made available for image
// acquisition.
int GrabNextImageByTrigger(INodeMap & nodeMap, CameraPtr pCam)
{
	int result = 0;

	try
	{
		// 
		// Use trigger to capture image
		//
		// *** NOTES ***
		// The software trigger only feigns being executed by the Enter key;
		// what might not be immediately apparent is that there is not a
		// continuous stream of images being captured; in other examples that 
		// acquire images, the camera captures a continuous stream of images. 
		// When an image is retrieved, it is plucked from the stream.
		//
		if (chosenTrigger == SOFTWARE)
		{
			// Get user input
			cout << "Press the Enter key to initiate software trigger." << endl;
			getchar();

			// Execute software trigger
			CCommandPtr ptrSoftwareTriggerCommand = nodeMap.GetNode("TriggerSoftware");
			if (!IsWritable(ptrSoftwareTriggerCommand))
			{
				cout << "Trigger not writeable..." << endl;
			}
			if (!IsAvailable(ptrSoftwareTriggerCommand) )
			{
				cout << "Trigger not available..." << endl;
			}
			if (!IsAvailable(ptrSoftwareTriggerCommand) || !IsWritable(ptrSoftwareTriggerCommand))
			{
				cout << "Unable to execute trigger. Aborting..." << endl;
				return -1;
			}

			ptrSoftwareTriggerCommand->Execute();

			// TODO: Blackfly and Flea3 GEV cameras need 2 second delay after software trigger 
		}
		else if (chosenTrigger == HARDWARE)
		{
			// Execute hardware trigger
			cout << "Use the hardware to trigger image acquisition." << endl;
		}
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}

	return result;
}

// Reset Trigger with CamList input
int ResetTrigger(CameraList camList)
{
	int result = 0;
	CameraPtr pCam = NULL;

	try
	{
		//
		// Turn trigger mode back off
		//
		// *** NOTES ***
		// Once all images have been captured, turn trigger mode back off to
		// restore the camera to a clean state.
		//
		for (int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);
			// Retrieve GenICam nodemap
			INodeMap & nodeMap = pCam->GetNodeMap();

			CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
			if (!IsAvailable(ptrTriggerMode) || !IsReadable(ptrTriggerMode))
			{
				cout << "Unable to disable trigger mode (node retrieval). Non-fatal error..." << endl;
				return -1;
			}
			CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
			if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
			{
				cout << "Unable to disable trigger mode (enum entry retrieval). Non-fatal error..." << endl;
				return -1;
			}
			ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());
			cout << "Trigger mode disabled..." << endl << endl;
		}
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}

	return result;
}


// This function returns the camera to a normal state by turning off trigger 
// mode.
//int ResetTrigger(INodeMap & nodeMap)
//{
//	int result = 0;
//
//	try
//	{
//		//
//		// Turn trigger mode back off
//		//
//		// *** NOTES ***
//		// Once all images have been captured, turn trigger mode back off to
//		// restore the camera to a clean state.
//		//
//		CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
//		if (!IsAvailable(ptrTriggerMode) || !IsReadable(ptrTriggerMode))
//		{
//			cout << "Unable to disable trigger mode (node retrieval). Non-fatal error..." << endl;
//			return -1;
//		}
//
//		CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
//		if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
//		{
//			cout << "Unable to disable trigger mode (enum entry retrieval). Non-fatal error..." << endl;
//			return -1;
//		}
//
//		ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());
//
//		cout << "Trigger mode disabled..." << endl << endl;
//	}
//	catch (Spinnaker::Exception &e)
//	{
//		cout << "Error: " << e.what() << endl;
//		result = -1;
//	}
//
//	return result;
//}

// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.
int PrintDeviceInfo(INodeMap & nodeMap, unsigned int camNum)
{
	int result = 0;

	cout << "Printing device information for camera " << camNum << "..." << endl << endl;

	FeatureList_t features;
	CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
	if (IsAvailable(category) && IsReadable(category))
	{
		category->GetFeatures(features);

		FeatureList_t::const_iterator it;
		for (it = features.begin(); it != features.end(); ++it)
		{
			CNodePtr pfeatureNode = *it;
			cout << pfeatureNode->GetName() << " : ";
			CValuePtr pValue = (CValuePtr)pfeatureNode;
			cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
			cout << endl;
		}
	}
	else
	{
		cout << "Device control information not available." << endl;
	}
	cout << endl;

	return result;
}

// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.
//int PrintDeviceInfo(INodeMap & nodeMap)
//{
//	int result = 0;
//
//	cout << endl << "*** DEVICE INFORMATION ***" << endl << endl;
//
//	try
//	{
//		FeatureList_t features;
//		CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
//		if (IsAvailable(category) && IsReadable(category))
//		{
//			category->GetFeatures(features);
//
//			FeatureList_t::const_iterator it;
//			for (it = features.begin(); it != features.end(); ++it)
//			{
//				CNodePtr pfeatureNode = *it;
//				cout << pfeatureNode->GetName() << " : ";
//				CValuePtr pValue = (CValuePtr)pfeatureNode;
//				cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
//				cout << endl;
//			}
//		}
//		else
//		{
//			cout << "Device control information not available." << endl;
//		}
//	}
//	catch (Spinnaker::Exception &e)
//	{
//		cout << "Error: " << e.what() << endl;
//		result = -1;
//	}
//
//	return result;
//}

// This function acquires and saves 10 images from each device.  
int AcquireImages(CameraList camList)
{
	int result = 0;
	CameraPtr pCam = NULL;
	CameraPtr pCam0 = NULL;

	cout << endl << "*** IMAGE ACQUISITION ***" << endl << endl;

	try
	{
		//
		// Prepare each camera to acquire images
		// 
		// *** NOTES ***
		// For pseudo-simultaneous streaming, each camera is prepared as if it 
		// were just one, but in a loop. Notice that cameras are selected with 
		// an index. We demonstrate pseduo-simultaneous streaming because true 
		// simultaneous streaming would require multiple process or threads,
		// which is too complex for an example. 
		// 
		// Serial numbers are the only persistent objects we gather in this
		// example, which is why a vector is created.
		//
		vector<gcstring> strSerialNumbers(camList.GetSize());

		for (int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);

			// Set acquisition mode to continuous
			CEnumerationPtr ptrAcquisitionMode = pCam->GetNodeMap().GetNode("AcquisitionMode");
			if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
			{
				cout << "Unable to set acquisition mode to continuous (node retrieval; camera " << i << "). Aborting..." << endl << endl;
				return -1;
			}
			CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
			if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
			{
				cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval " << i << "). Aborting..." << endl << endl;
				return -1;
			}
			int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
			ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
			cout << "Camera " << i << " acquisition mode set to continuous..." << endl;

			// Begin acquiring images
			pCam->BeginAcquisition();
			cout << "Camera " << i << " started acquiring images..." << endl;

			// Retrieve device serial number for filename
			strSerialNumbers[i] = "";
			CStringPtr ptrStringSerial = pCam->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");
			if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
			{
				strSerialNumbers[i] = ptrStringSerial->GetValue();
				cout << "Camera " << i << " serial number set to " << strSerialNumbers[i] << "..." << endl;
			}
			cout << endl;
		}

		//
		// Retrieve, convert, and save images for each camera
		//
		// *** NOTES ***
		// In order to work with simultaneous camera streams, nested loops are
		// needed. It is important that the inner loop be the one iterating
		// through the cameras; otherwise, all images will be grabbed from a
		// single camera before grabbing any images from another.
		//
		const unsigned int k_numImages = 10;

		for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++)
		{
			//// Select camera
			//pCam0 = camList.GetByIndex(1);

			//// Retrieve TL device nodemap
			//INodeMap & nodeMap0 = pCam0->GetTLDeviceNodeMap();

			//// Retrieve the next image from the trigger
			//result = GrabNextImageByTrigger(nodeMap0, pCam0);

			cout << "Press the Enter key to initiate software trigger." << endl;
			getchar();

			for (int i = 0; i < camList.GetSize(); i++)
			{

				try
				{
					// Select camera
					pCam = camList.GetByIndex(i);

					// Retrieve TL device nodemap
					INodeMap & nodeMap = pCam->GetTLDeviceNodeMap();

					// Retrieve next received image and ensure image completion
					ImagePtr pResultImage = pCam->GetNextImage();

					if (pResultImage->IsIncomplete())
					{
						cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl << endl;
					}
					else
					{
						// Print image information
						cout << "Camera " << i << " grabbed image " << imageCnt << ", width = " << pResultImage->GetWidth() << ", height = " << pResultImage->GetHeight() << endl;

						// Convert image to mono 8
						ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);

						// Create a unique filename
						ostringstream filename;
						filename << "AcquisitionMultipleCamera-";
						if (strSerialNumbers[i] != "")
						{
							filename << strSerialNumbers[i].c_str();
						}
						else
						{
							filename << i;
						}
						filename << "-" << imageCnt << ".jpg";

						// Save image
						convertedImage->Save(filename.str().c_str());
						cout << "Image saved at " << filename.str() << endl;
					}

					// Release image
					pResultImage->Release();
					cout << endl;
				}
				catch (Spinnaker::Exception &e)
				{
					cout << "Error: " << e.what() << endl;
					result = -1;
				}
			}
		}

		//
		// End acquisition for each camera
		//
		// *** NOTES ***
		// Notice that what is usually a one-step process is now two steps
		// because of the additional step of selecting the camera. It is worth
		// repeating that camera selection needs to be done once per loop.
		//
		// It is possible to interact with cameras through the camera list with
		// GetByIndex(); this is an alternative to retrieving cameras as 
		// CameraPtr objects that can be quick and easy for small tasks.
		//
		for (int i = 0; i < camList.GetSize(); i++)
		{
			// End acquisition
			camList.GetByIndex(i)->EndAcquisition();
		}
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}

	return result;
}




// This function acquires and saves 10 images from a device; please see
// Acquisition example for more in-depth comments on acquiring images.
//int AcquireImages(CameraPtr pCam, INodeMap & nodeMap, INodeMap & nodeMapTLDevice)
//{
//	int result = 0;
//
//	cout << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
//
//	try
//	{
//		// Set acquisition mode to continuous
//		CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
//		if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
//		{
//			cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
//			return -1;
//		}
//		CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
//		if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
//		{
//			cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting..." << endl << endl;
//			return -1;
//		}
//		int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
//		ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
//		cout << "Acquisition mode set to continuous..." << endl;
//
//		// Begin acquiring images
//		pCam->BeginAcquisition();
//		cout << "Acquiring images..." << endl;
//
//		// Retrieve device serial number for filename
//		gcstring deviceSerialNumber("");
//		CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
//		if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
//		{
//			deviceSerialNumber = ptrStringSerial->GetValue();
//
//			cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
//		}
//		cout << endl;
//
//		// Retrieve, convert, and save images
//		const int unsigned k_numImages = 10;
//		for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++)
//		{
//			try
//			{
//				// Retrieve the next image from the trigger
//				result = result | GrabNextImageByTrigger(nodeMap, pCam);
//
//				// Retrieve the next received image
//				ImagePtr pResultImage = pCam->GetNextImage();
//				if (pResultImage->IsIncomplete())
//				{
//					cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl << endl;
//				}
//				else
//				{
//					// Print image information
//					cout << "Grabbed image " << imageCnt << ", width = " << pResultImage->GetWidth() << ", height = " << pResultImage->GetHeight() << endl;
//					// Convert image to mono 8
//					ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);
//					// Create a unique filename
//					ostringstream filename;
//					filename << "Trigger-";
//					if (deviceSerialNumber != "")
//					{
//						filename << deviceSerialNumber.c_str() << "-";
//					}
//					filename << imageCnt << ".jpg";
//					// Save image
//					convertedImage->Save(filename.str().c_str());
//					cout << "Image saved at " << filename.str() << endl;
//				}
//
//				// Release image
//				pResultImage->Release();
//
//				cout << endl;
//			}
//			catch (Spinnaker::Exception &e)
//			{
//				cout << "Error: " << e.what() << endl;
//				result = -1;
//			}
//		}
//
//		// End acquisition
//		pCam->EndAcquisition();
//	}
//	catch (Spinnaker::Exception &e)
//	{
//		cout << "Error: " << e.what() << endl;
//		result = -1;
//	}
//
//	return result;
//}

// This function acts as the body of the example; please see NodeMapInfo example 
// for more in-depth comments on setting up cameras.
//int RunSingleCamera(CameraPtr pCam)
//{
//	int result = 0;
//	int err = 0;
//
//	try
//	{
//		// Retrieve TL device nodemap and print device information
//		INodeMap & nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
//
//		result = PrintDeviceInfo(nodeMapTLDevice);
//		
//		// Initialize camera
//		pCam->Init();
//
//		// Retrieve GenICam nodemap
//		INodeMap & nodeMap = pCam->GetNodeMap();
//
//		//// Configure trigger
//		//err = ConfigureTrigger(nodeMap);
//		//if (err < 0)
//		//{
//		//	return err;
//		//}
//
//		// Acquire images
//		result = result | AcquireImages(pCam, nodeMap, nodeMapTLDevice);
//
//		// Reset trigger
//		result = result | ResetTrigger(nodeMap);
//
//		// Deinitialize camera
//		pCam->DeInit();
//	}
//	catch (Spinnaker::Exception &e)
//	{
//		cout << "Error: " << e.what() << endl;
//		result = -1;
//	}
//
//	return result;
//}


int RunMultipleCameras(CameraList camList)
{
	int result = 0;
	CameraPtr pCam = NULL;

	try
	{
		//
		// Retrieve transport layer nodemaps and print device information for 
		// each camera
		//
		// *** NOTES ***
		// This example retrieves information from the transport layer nodemap 
		// twice: once to print device information and once to grab the device 
		// serial number. Rather than caching the nodemap, each nodemap is 
		// retrieved both times as needed.
		//
		cout << endl << "*** DEVICE INFORMATION ***" << endl << endl;

		for (int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);
			// Retrieve TL device nodemap
			INodeMap & nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
			// Print device information
			result = PrintDeviceInfo(nodeMapTLDevice, i);

			// Initialize camera
			pCam->Init();

			int err = 0;
			// Retrieve GenICam nodemap
			INodeMap & nodeMap = pCam->GetNodeMap();
			// Configure trigger
			err = ConfigureTrigger(nodeMap);
			if (err < 0)
			{
				return err;
			}
		}


		// Acquire images on all cameras
		result = result | AcquireImages(camList);

		// Acquire images on all cameras
		result = result | ResetTrigger(camList);

		//// Reset trigger for each camera
		//for (int i = 0; i < camList.GetSize(); i++)
		//{
		//	// Select camera
		//	pCam = camList.GetByIndex(i);
		//	// Retrieve TL device nodemap
		//	INodeMap & nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
		//	// Reset trigger
		//	result = result | ResetTrigger(nodeMapTLDevice);
		//}

		// Deinitialize each camera
		for (int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);
			// Deinitialize camera
			pCam->DeInit();
		}
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}

	return result;
}


// Example entry point; please see Enumeration example for more in-depth 
// comments on preparing and cleaning up the system.
int main(int /*argc*/, char** /*argv*/)
{
	// Since this application saves images in the current folder
	// we must ensure that we have permission to write to this folder.
	// If we do not have permission, fail right away.
	FILE *tempFile = fopen("test.txt", "w+");
	if (tempFile == NULL)
	{
		cout << "Failed to create file in current folder.  Please check "
			"permissions."
			<< endl;
		cout << "Press Enter to exit..." << endl;
		getchar();
		return -1;
	}
	fclose(tempFile);
	remove("test.txt");

	int result = 0;

	// Print application build information
	cout << "Application build date: " << __DATE__ << " " << __TIME__ << endl << endl;

	// Retrieve singleton reference to system object
	SystemPtr system = System::GetInstance();

	// Retrieve list of cameras from the system
	CameraList camList = system->GetCameras();

	unsigned int numCameras = camList.GetSize();

	cout << "Number of cameras detected: " << numCameras << endl << endl;

	// Finish if there are no cameras
	if (numCameras == 0)
	{
		// Clear camera list before releasing system
		camList.Clear();
		// Release system
		system->ReleaseInstance();
		cout << "Not enough cameras!" << endl;
		cout << "Done! Press Enter to exit..." << endl;
		getchar();
		return -1;
	}

	// Run example on all cameras
	cout << endl << "Running example for all cameras..." << endl;

	result = RunMultipleCameras(camList);

	cout << "Example complete..." << endl << endl;

	//// Run example on each camera
	//for (unsigned int i = 0; i < numCameras; i++)
	//{
	//	cout << endl << "Running example for camera " << i << "..." << endl;
	//	result = result | RunSingleCamera(camList.GetByIndex(i));
	//	cout << "Camera " << i << " example complete..." << endl << endl;
	//}

	// Clear camera list before releasing system
	camList.Clear();

	// Release system
	system->ReleaseInstance();

	cout << endl << "Done! Press Enter to exit..." << endl;
	getchar();

	return result;
}

