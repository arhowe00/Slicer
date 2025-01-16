import os

import qt
import vtk

import slicer

from . import DataProbeUtil

LAYER_FOREGROUND = 0
LAYER_BACKGROUND = 1
LAYER_LABEL = 2

class DICOMAnnotationPropertyValueProvider:
    # Static in abstract provider
    @staticmethod
    def GetLayerValueAsInteger(attributes):
        if "layer" in attributes:
            layer = attributes["layer"]

            def get_digit(s):
                return int(s) if s.isdigit() else -1

            if layer == "foreground" or get_digit(layer) == LAYER_FOREGROUND:
                return LAYER_FOREGROUND
            elif layer == "background" or get_digit(layer) == LAYER_BACKGROUND:
                return LAYER_BACKGROUND
            elif layer == "label" or get_digit(layer) == LAYER_LABEL:
                return LAYER_LABEL
        return -1

    # A part of the default annotation provider, not abstract. We need a way to
    # get app logic
    @staticmethod
    def GetAppLogic():
        return slicer.app.applicationLogic()

    # To be registered
    @staticmethod
    def CanProvideValueForPropertyName(propertyName):
        return propertyName in DICOMAnnotationPropertyValueProvider.GetSupportedProperties()

    @staticmethod
    def GetValueForPropertyName(propertyName, attributes, sliceNode):

        # Do not attempt to retrieve dicom values if no local database exists
        if not slicer.dicomDatabase.isOpen:
            return ""

        # Get the layers
        sliceLogic = DICOMAnnotationPropertyValueProvider.GetAppLogic().GetSliceLogic(sliceNode)
        backgroundVolume = sliceLogic.GetBackgroundLayer().GetVolumeNode()
        foregroundVolume = sliceLogic.GetForegroundLayer().GetVolumeNode()

        # Case I: Both background and foreground
        output = ""
        if backgroundVolume is not None and foregroundVolume is not None:
            bgUids = backgroundVolume.GetAttribute("DICOM.instanceUIDs")
            fgUids = foregroundVolume.GetAttribute("DICOM.instanceUIDs")
            if bgUids and fgUids:
                bgUid = bgUids.partition(" ")[0]
                fgUid = fgUids.partition(" ")[0]

                backgroundDicomDic = DICOMAnnotationPropertyValueProvider.extractDICOMValues(bgUid)
                foregroundDicomDic = DICOMAnnotationPropertyValueProvider.extractDICOMValues(fgUid)

                if not DICOMAnnotationPropertyValueProvider.uidsMatch(bgUid, fgUid):
                    return ""

                backgroundDicomDic["Patient Birth Date"] = backgroundDicomDic["Patient Birth Date"]

                properties = {
                    "SeriesDate": "Series Date",
                    "SeriesTime": "Series Time",
                    "SeriesDescription": "Series Description",
                }

                if propertyName in properties:
                    key = properties[propertyName]
                    if backgroundDicomDic[key] != foregroundDicomDic[key]:
                        layer_value = DICOMAnnotationPropertyValueProvider.GetLayerValueAsInteger(attributes)
                        if layer_value is LAYER_BACKGROUND:
                            output = "B: " + output
                        elif layer_value is LAYER_FOREGROUND:
                            output = "F: " + output

                uid = bgUid  # Will be used for later down the line
            elif bgUids and DICOMAnnotationPropertyValueProvider.backgroundDICOMAnnotationsPersistence:
                uid = bgUids.partition(" ")[0]
            else:
                return ""
        elif backgroundVolume is not None:
            uids = backgroundVolume.GetAttribute("DICOM.instanceUIDs")
            if uids:
                uid = uids.partition(" ")[0]
        elif foregroundVolume is not None:
            uids = foregroundVolume.GetAttribute("DICOM.instanceUIDs")
            if uids:
                uid = uids.partition(" ")[0]

        dicomDic = DICOMAnnotationPropertyValueProvider.extractDICOMValues(uid)

        if propertyName == "PatientName":
            output += dicomDic["Patient Name"].replace("^", ", ")
        elif propertyName == "PatientID":
            output += "ID: " + dicomDic["Patient ID"]
        elif propertyName == "PatientBirthDate":
            output += DICOMAnnotationPropertyValueProvider.formatDICOMDate(dicomDic["Patient Birth Date"])
        elif propertyName == "PatientInfo":
            output += DICOMAnnotationPropertyValueProvider.makePatientInfo(dicomDic)
        elif propertyName == "SeriesDate":
            output += DICOMAnnotationPropertyValueProvider.formatDICOMDate(dicomDic["Series Date"])
        elif propertyName == "SeriesTime":
            output += DICOMAnnotationPropertyValueProvider.formatDICOMTime(dicomDic["Series Time"])
        elif propertyName == "SeriesDescription":
            output += dicomDic["Series Description"]
        elif propertyName == "InstitutionName":
            output += dicomDic["Institution Name"]
        elif propertyName == "ReferringPhysician":
            output += dicomDic["Referring Physician Name"].replace("^", ", ")
        elif propertyName == "Manufacturer":
            output += dicomDic["Manufacturer"]
        elif propertyName == "Model":
            output += dicomDic["Model"]
        elif propertyName == "Patient-Position":
            output += dicomDic["Patient Position"]
        elif propertyName == "MR":
            output += "TR: " + dicomDic["Repetition Time"]
        elif propertyName == "TE":
            output += "TE: " + dicomDic["Echo Time"]

        return output

    @staticmethod
    def GetSupportedProperties():
        return {
            "PatientName",
            "PatientID",
            "PatientInfo",
            "SeriesDate",
            "SeriesTime",
            "SeriesDescription",
            "InstitutionName",
            "ReferringPhysician",
            "Manufacturer",
            "Model",
            "Patient-Position",
            "TR",
            "TE",
        }

    # Implementation helper routines
    @staticmethod
    def uidsMatch(backgroundDicomDic, foregroundDicomDic):
        return (
            backgroundDicomDic["Patient Name"] == foregroundDicomDic["Patient Name"]
            and backgroundDicomDic["Patient ID"] == foregroundDicomDic["Patient ID"]
            and backgroundDicomDic["Patient Birth Date"] == foregroundDicomDic["Patient Birth Date"]
        )

    @staticmethod
    def formatDICOMDate(date):
        standardDate = ""
        if date != "":
            date = date.rstrip()
            # convert to ISO 8601 Date format
            standardDate = date[:4] + "-" + date[4:6] + "-" + date[6:]
        return standardDate

    @staticmethod
    def formatDICOMTime(time):
        if time == "":
            # time field is empty
            return ""
        studyH = time[:2]
        if int(studyH) > 12:
            studyH = str(int(studyH) - 12)
            clockTime = " PM"
        else:
            studyH = studyH
            clockTime = " AM"
        studyM = time[2:4]
        studyS = time[4:6]
        return studyH + ":" + studyM + ":" + studyS + clockTime

    @staticmethod
    def extractDICOMValues(uid):
        p = {}
        tags = {
            "0008,0021": "Series Date",
            "0008,0031": "Series Time",
            "0008,0060": "Modality",
            "0008,0070": "Manufacturer",
            "0008,0080": "Institution Name",
            "0008,0090": "Referring Physician Name",
            "0008,103e": "Series Description",
            "0008,1090": "Model",
            "0010,0010": "Patient Name",
            "0010,0020": "Patient ID",
            "0010,0030": "Patient Birth Date",
            "0010,0040": "Patient Sex",
            "0010,1010": "Patient Age",
            "0018,5100": "Patient Position",
            "0018,0080": "Repetition Time",
            "0018,0081": "Echo Time",
        }
        for tag, tag_value in tags.items():
            value = slicer.dicomDatabase.instanceValue(uid, tag)
            p[tag_value] = value
        return p
