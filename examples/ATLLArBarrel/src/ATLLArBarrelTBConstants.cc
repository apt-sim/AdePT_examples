//**************************************************
// \file ATLLArBarrelTBConstants.cc
// \brief: implementation of ATLLArBarrelTBConstants 
//         namespace methods
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 6 May 2023
//**************************************************

//Includers from project files
//
#include "ATLLArBarrelTBConstants.hh"

using namespace ATLLArBarrelTBConstants;

std::ostream& ATLLArBarrelTBConstants::operator<<(std::ostream& ostream, const STACSection& section){
    switch (section) {
        case STACSection::Front:
            ostream << "Front Section";
            break;
        case STACSection::Middle:
            ostream << "Middle Section";
            break;
        case STACSection::Back:
            ostream << "Back Section";
            break;
    }
    return ostream;
}

//**************************************************
