#include "../headers/configHandler.h"  

configHandler::configHandler()
{
    
}

configHandler::~configHandler()
{
    
}

void configHandler::initMaps()
{
    //!SD.begin(chipSelectPin_);
    setObjectIDs();
}

void configHandler::setObjectIDs() // sets ID's for every object as they exist in config file
{
    Automode_gearMap_.ID = String("Automode_wanted_gear_map"); Automode_gearMap_.minData = 0; Automode_gearMap_.maxData = 200;
    Shift_solenoid_time_map_.ID = String("Shift_solenoid_time_map"); Shift_solenoid_time_map_.minData = 200; Shift_solenoid_time_map_.maxData = 2500;
    MPC_regularDrive_.ID = String("MPC_normalDrive"); MPC_regularDrive_.minData = 0; MPC_regularDrive_.maxData = 100;

    MPC_1to2_load_.ID = String("MPC_1to2_load"); MPC_1to2_load_.minData = 0; MPC_1to2_load_.maxData = 100;
    MPC_2to3_load_.ID = String("MPC_2to3_load"); MPC_2to3_load_.minData = 0; MPC_2to3_load_.maxData = 100;
    MPC_3to4_load_.ID = String("MPC_3to4_load"); MPC_3to4_load_.minData = 0; MPC_3to4_load_.maxData = 100;
    MPC_4to5_load_.ID = String("MPC_4to5_load"); MPC_4to5_load_.minData = 0; MPC_4to5_load_.maxData = 100;
    MPC_5to4_load_.ID = String("MPC_5to4_load"); MPC_5to4_load_.minData = 0; MPC_5to4_load_.maxData = 100;
    MPC_4to3_load_.ID = String("MPC_4to3_load"); MPC_4to3_load_.minData = 0; MPC_4to3_load_.maxData = 100;
    MPC_3to2_load_.ID = String("MPC_3to2_load"); MPC_3to2_load_.minData = 0; MPC_3to2_load_.maxData = 100;
    MPC_2to1_load_.ID = String("MPC_2to1_load"); MPC_2to1_load_.minData = 0; MPC_2to1_load_.maxData = 100;
    MPC_1to2_coast_.ID = String("MPC_1to2_coast"); MPC_1to2_coast_.minData = 0; MPC_1to2_coast_.maxData = 100;
    MPC_2to3_coast_.ID = String("MPC_2to3_coast"); MPC_2to3_coast_.minData = 0; MPC_2to3_coast_.maxData = 100;
    MPC_3to4_coast_.ID = String("MPC_3to4_coast"); MPC_3to4_coast_.minData = 0; MPC_3to4_coast_.maxData = 100;
    MPC_4to5_coast_.ID = String("MPC_4to5_coast"); MPC_4to5_coast_.minData = 0; MPC_4to5_coast_.maxData = 100;
    MPC_5to4_coast_.ID = String("MPC_5to4_coast"); MPC_5to4_coast_.minData = 0; MPC_5to4_coast_.maxData = 100;
    MPC_4to3_coast_.ID = String("MPC_4to3_coast"); MPC_4to3_coast_.minData = 0; MPC_4to3_coast_.maxData = 100;
    MPC_3to2_coast_.ID = String("MPC_3to2_coast"); MPC_3to2_coast_.minData = 0; MPC_3to2_coast_.maxData = 100;
    MPC_2to1_coast_.ID = String("MPC_2to1_coast"); MPC_2to1_coast_.minData = 0; MPC_2to1_coast_.maxData = 100;

    SPC_1to2_load_.ID = String("SPC_1to2_load"); SPC_1to2_load_.minData = 0; SPC_1to2_load_.maxData = 100;
    SPC_2to3_load_.ID = String("SPC_2to3_load"); SPC_2to3_load_.minData = 0; SPC_2to3_load_.maxData = 100;
    SPC_3to4_load_.ID = String("SPC_3to4_load"); SPC_3to4_load_.minData = 0; SPC_3to4_load_.maxData = 100;
    SPC_4to5_load_.ID = String("SPC_4to5_load"); SPC_4to5_load_.minData = 0; SPC_4to5_load_.maxData = 100;
    SPC_5to4_load_.ID = String("SPC_5to4_load"); SPC_5to4_load_.minData = 0; SPC_5to4_load_.maxData = 100;
    SPC_4to3_load_.ID = String("SPC_4to3_load"); SPC_4to3_load_.minData = 0; SPC_4to3_load_.maxData = 100;
    SPC_3to2_load_.ID = String("SPC_3to2_load"); SPC_3to2_load_.minData = 0; SPC_3to2_load_.maxData = 100;
    SPC_2to1_load_.ID = String("SPC_2to1_load"); SPC_2to1_load_.minData = 0; SPC_2to1_load_.maxData = 100;
    SPC_1to2_coast_.ID = String("SPC_1to2_coast"); SPC_1to2_coast_.minData = 0; SPC_1to2_coast_.maxData = 100;
    SPC_2to3_coast_.ID = String("SPC_2to3_coast"); SPC_2to3_coast_.minData = 0; SPC_2to3_coast_.maxData = 100;
    SPC_3to4_coast_.ID = String("SPC_3to4_coast"); SPC_3to4_coast_.minData = 0; SPC_3to4_coast_.maxData = 100;
    SPC_4to5_coast_.ID = String("SPC_4to5_coast"); SPC_4to5_coast_.minData = 0; SPC_4to5_coast_.maxData = 100;
    SPC_5to4_coast_.ID = String("SPC_5to4_coast"); SPC_5to4_coast_.minData = 0; SPC_5to4_coast_.maxData = 100;
    SPC_4to3_coast_.ID = String("SPC_4to3_coast"); SPC_4to3_coast_.minData = 0; SPC_4to3_coast_.maxData = 100;
    SPC_3to2_coast_.ID = String("SPC_3to2_coast"); SPC_3to2_coast_.minData = 0; SPC_3to2_coast_.maxData = 100;
    SPC_2to1_coast_.ID = String("SPC_2to1_coast"); SPC_2to1_coast_.minData = 0; SPC_2to1_coast_.maxData = 100;

    shiftTimeTargetMap_.ID = String("Shift_time_target_map"); shiftTimeTargetMap_.minData = 200; shiftTimeTargetMap_.maxData = 2000;
    engSpdLoadFactorMap_.ID = String("Eng_spd_load_factor"); engSpdLoadFactorMap_.minData = 0; engSpdLoadFactorMap_.maxData = 100;
    MAPLinearizationMap_.ID = String("MAP_linearization_map"); MAPLinearizationMap_.minData = 0; MAPLinearizationMap_.maxData = 5000;
    TPSLinearizationMap_.ID = String("TPS_linearization_map"); TPSLinearizationMap_.minData = 0; TPSLinearizationMap_.maxData = 5000;
    oilTempMap_.ID = String("ATF_temp_sensor_linearization"); oilTempMap_.minData = 0; oilTempMap_.maxData = 3000;

    startWith1StGear_.ID = String("Start_with_1St_gear"); startWith1StGear_.minData = 0; startWith1StGear_.maxData = 1;
    minimumVehicleSpeedForGearRatioDetection_.ID = String("Min_vehicle_spd_for_gear_ratio_detection"); minimumVehicleSpeedForGearRatioDetection_.minData = 0; minimumVehicleSpeedForGearRatioDetection_.maxData = 270;
    wheelCircum_.ID = String("Wheel_circum"); wheelCircum_.minData = 1000; wheelCircum_.maxData = 3000;
    brightness_.ID = String("Screen_brightness"); brightness_.minData = 0; brightness_.maxData = 100;
    engineSpeedPulsesPerRev_.ID = String("engineSpeed_Pulses_Per_Revolution"); engineSpeedPulsesPerRev_.minData = 1; engineSpeedPulsesPerRev_.maxData = 100;
    driveShaftPulsesPerRev_.ID = String("driveShaft_Pulses_Per_Revolution"); driveShaftPulsesPerRev_.minData = 1; driveShaftPulsesPerRev_.maxData = 100;
    tccControlPfactor_.ID = String("TCC_PI_control_P_factor"); tccControlPfactor_.minData = 1; tccControlPfactor_.maxData = 1000;
    tccControlIfactor_.ID = String("TCC_PI_control_I_factor"); tccControlIfactor_.minData = 1; tccControlIfactor_.maxData = 1000;
}

void configHandler::printObjectsToSerial()
{
    Serial.print("Parametrit: \n");
    for (int i = 0; i < parametersAmount_; i++)
    {
        Serial.print(parameters_[i]->ID);
        Serial.print(": ");
        Serial.print(parameters_[i]->data);
        Serial.print(", luettu: ");
        Serial.println(parameters_[i]->readFromSD);
    }

    Serial.print("\nSinkkukartat: \n");
    for (int i = 0; i < singleAxisMapsAmount_; i++)
    {
        Serial.print(singleAxisMaps_[i]->ID);
        Serial.print(", sarakkeita: ");
        Serial.print(singleAxisMaps_[i]->columns);
        Serial.print(", luettu: ");
        Serial.println(singleAxisMaps_[i]->readFromSD);
    }
 
    Serial.print("\nKaksiakselikartat:\n ");
    for (int i = 0; i < dualAxisMapsAmount_; i++)
    {
        Serial.print(dualAxisMaps_[i]->ID);
        Serial.print(", rivejä: ");
        Serial.print(dualAxisMaps_[i]->rows);
        Serial.print(", sarakkeita: ");
        Serial.print(dualAxisMaps_[i]->columns);
        Serial.print(", luettu: ");
        Serial.println(dualAxisMaps_[i]->readFromSD);
    } 
}

int configHandler::giveShiftMapValue(shiftType stype, driveType dtype, int oil, int load)
{
    dualAxisMap *map = identifyShiftMap(stype, dtype);
    lastShiftOilTemp_ = oil;
    lastShiftLoad_ = load;
    lastShiftDriveType_ = dtype;
    
    if (stype == MPC_1to2 || stype == MPC_2to3 || stype == MPC_3to4 || stype == MPC_4to5 || stype == MPC_5to4 || stype == MPC_4to3 ||stype == MPC_3to2 || stype == MPC_2to1)
    {        
        lastMPCmap_ = map;
        lastMPCval_ = readDualAxisMap(map, oil, load);
        lastShiftMPCType_ = stype; /// VAIN TESTAUKSEEN
        return lastMPCval_;
    }
    else
    {
        lastSPCmap_ = map;
        lastSPCval_ = readDualAxisMap(map, oil, load);
        lastShiftSPCType_ = stype; /// VAIN TESTAUKSEEN
        return lastSPCval_;
    }
}

void configHandler::changeShiftMapValue(dualAxisMap* map, int rowVal, int colVal, int newVal)
{
    int i = 0;
    if (rowVal <= map->rowTitles[0]) // smaller or as small as the smallest in row
    { 
        i = 0;
    }
    else if(rowVal >= map->rowTitles[map->rows-1]) // bigger or as big as the biggest in row
    {
        i = map->rows-1;     
    }
    else // in between
    {
        for(i = 0; i < map->rows; i++)
        {
            if (rowVal <= map->rowTitles[i])
            {
                if (rowVal <  (map->rowTitles[i-1] + ((map->rowTitles[i] - map->rowTitles[i-1]) / 2))) // if smaller than halfway of two row titles, then use previous title
                {
                    i--;
                }
                break;
            }
        } 
    }

    int j = 0;
    if (colVal <= map->columnTitles[0]) // smaller or as small as the smallest in column
    { 
        j = 0;

    }
    else if(colVal >= map->columnTitles[map->columns-1]) // bigger or as big as the biggest in column
    {
        j = map->columns-1;     
    }
    else // in between
    {
        for(j = 0; j < map->columns; j++)
        {
            if (colVal <= map->columnTitles[j])
            {
                if (colVal <  (map->columnTitles[j-1] + ((map->columnTitles[j] - map->columnTitles[j-1]) / 2))) // if smaller than halfway of two column titles, then use previous title
                {
                    j--;
                }
                break;
            }
        }  
    }
    map->data[i][j] = newVal;
}

void configHandler::modifyLastShiftMaps(int MPCchange, int SPCchange)
{
    if (lastMPCval_ + MPCchange > lastMPCmap_->maxData) // if too big value
    {
        changeShiftMapValue(lastMPCmap_, lastShiftOilTemp_, lastShiftLoad_, lastMPCmap_->maxData);
    }
    else if (lastMPCval_ + MPCchange < lastMPCmap_->minData) // if too small value
    {
        changeShiftMapValue(lastMPCmap_, lastShiftOilTemp_, lastShiftLoad_, lastMPCmap_->minData);
    }
    else
    {
        changeShiftMapValue(lastMPCmap_, lastShiftOilTemp_, lastShiftLoad_, lastMPCval_ + MPCchange);
    }
    
    if (lastSPCval_ + SPCchange > lastSPCmap_->maxData)
    {
        changeShiftMapValue(lastSPCmap_, lastShiftOilTemp_, lastShiftLoad_, lastSPCmap_->maxData);
    }
    else if (lastSPCval_ + SPCchange < lastSPCmap_->minData) // if too small value
    {
        changeShiftMapValue(lastSPCmap_, lastShiftOilTemp_, lastShiftLoad_, lastSPCmap_->minData);
    }
    else
    {
        changeShiftMapValue(lastSPCmap_, lastShiftOilTemp_, lastShiftLoad_, lastSPCval_ + SPCchange);
    }   

    Serial.print(identifyShiftMap(lastShiftMPCType_, lastShiftDriveType_)->ID);
    Serial.print(": ");
    Serial.print(giveShiftMapValue(lastShiftMPCType_, lastShiftDriveType_, lastShiftOilTemp_, lastShiftLoad_));
    Serial.print(" ");
    Serial.print(identifyShiftMap(lastShiftSPCType_, lastShiftDriveType_)->ID);
    Serial.print(": ");
    Serial.println(giveShiftMapValue(lastShiftSPCType_, lastShiftDriveType_, lastShiftOilTemp_, lastShiftLoad_));
}

uint8_t configHandler::giveAutoModeTargetGear(int vehicleSpeed, uint8_t currentGear, int rowVal)
{
    dualAxisMap *map = &Automode_gearMap_;
    int i = 0;
    if (rowVal <= map->rowTitles[0]) // smaller or as small as the smallest in row
    { 
        i = 0;
    }
    else if(rowVal >= map->rowTitles[map->rows-1]) // bigger or as big as the biggest in row
    {
        i = map->rows-1;     
    }
    else // in between
    {
        for(i = 0; i < map->rows; i++)
        {
            if (rowVal <= map->rowTitles[i])
            {
                if (rowVal <  (map->rowTitles[i-1] + ((map->rowTitles[i] - map->rowTitles[i-1]) / 2))) // if smaller than halfway of two row titles, then use previous title
                {
                    i--;
                }
                break;
            }
        } 
    }

    //first to second
    if (currentGear == 1 && map->data[i][0] <= vehicleSpeed)
    {
        return 2;
    }

    //second to first
    else if (currentGear == 2 && map->data[i][1] >= vehicleSpeed)
    {
        return 1;
    }            

    //second to third            
    else if (currentGear == 2 && map->data[i][2] <= vehicleSpeed)
    {
        return 3;
    }

    //third to second
    else if (currentGear == 3 && map->data[i][3] >= vehicleSpeed)
    {
        return 2;
    }

    //third to fourth
    else if (currentGear == 3 && map->data[i][4] <= vehicleSpeed)
    {
        return 4;
    }

    //fourth to third
    else if (currentGear == 4 && map->data[i][5] >= vehicleSpeed)
    {
        return 3;
    }             

    //fourth to fifth
    else if (currentGear == 4 && map->data[i][6] <= vehicleSpeed)
     {
        return 5;
     }
    
    //fifth to fourth
    else if (currentGear == 5 && map->data[i][7] >= vehicleSpeed)
    {
        return 4;
    }
    else // return 0 if there is no need to shift
    {
        return 0;
    }    
}

int configHandler::giveRegularMPCMapValue(int rowVal, int colVal)
{
    return readDualAxisMap(&MPC_regularDrive_, rowVal, colVal);
}
int configHandler::giveOilTempValue(int resistance)
{
    return readInverseSingleAxisMap(&oilTempMap_, resistance);
}

int configHandler::giveTPSValue(int voltage)
{
    return readInverseSingleAxisMap(&TPSLinearizationMap_, voltage);
}

int configHandler::giveMAPValue(int voltage)
{
    return readInverseSingleAxisMap(&MAPLinearizationMap_, voltage);
}

int configHandler::giveEngSpdLoadFactorValue(int engineSpeed)
{
    return readSingleAxisMap(&engSpdLoadFactorMap_, engineSpeed);
}

int configHandler::giveShiftTimeTargetValue(int load)
{
    return readSingleAxisMap(&shiftTimeTargetMap_, load);
}

struct configHandler::dualAxisMap* configHandler::identifyShiftMap(shiftType stype, driveType dtype)
{   
    switch(stype) {
        //MPC
        case MPC_1to2:
        if (dtype == load)
        {
            return &MPC_1to2_load_;
        }
        else
        {
            return &MPC_1to2_coast_;
        }        
        break;

        case MPC_2to3:
        if (dtype == load)
        {
            return &MPC_2to3_load_;
        }
        else
        {
            return &MPC_2to3_coast_;
        }        
        break;

        case MPC_3to4:
        if (dtype == load)
        {
            return &MPC_3to4_load_;
        }
        else
        {
            return &MPC_3to4_coast_;
        }        
        break;

        case MPC_4to5:
        if (dtype == load)
        {
            return &MPC_4to5_load_;
        }
        else
        {
            return &MPC_4to5_coast_;
        }        
        break;

        case MPC_5to4:
        if (dtype == load)
        {
            return &MPC_5to4_load_;
        }
        else
        {
            return &MPC_5to4_coast_;
        }        
        break;

        case MPC_4to3:
        if (dtype == load)
        {
            return &MPC_4to3_load_;
        }
        else
        {
            return &MPC_4to3_coast_;
        }        
        break;

        case MPC_3to2:
        if (dtype == load)
        {
            return &MPC_3to2_load_;
        }
        else
        {
            return &MPC_3to2_coast_;
        }        
        break;

        case MPC_2to1:
        if (dtype == load)
        {
            return &MPC_2to1_load_;
        }
        else
        {
            return &MPC_2to1_coast_;
        }        
        break;


        // SPC
        case SPC_1to2:
        if (dtype == load)
        {
            return &SPC_1to2_load_;
        }
        else
        {
            return &SPC_1to2_coast_;
        }        
        break;

        case SPC_2to3:
        if (dtype == load)
        {
            return &SPC_2to3_load_;
        }
        else
        {
            return &SPC_2to3_coast_;
        }        
        break;

        case SPC_3to4:
        if (dtype == load)
        {
            return &SPC_3to4_load_;
        }
        else
        {
            return &SPC_3to4_coast_;
        }        
        break;

        case SPC_4to5:
        if (dtype == load)
        {
            return &SPC_4to5_load_;
        }
        else
        {
            return &SPC_4to5_coast_;
        }        
        break;

        case SPC_5to4:
        if (dtype == load)
        {
            return &SPC_5to4_load_;
        }
        else
        {
            return &SPC_5to4_coast_;
        }        
        break;

        case SPC_4to3:
        if (dtype == load)
        {
            return &SPC_4to3_load_;
        }
        else
        {
            return &SPC_4to3_coast_;
        }        
        break;

        case SPC_3to2:
        if (dtype == load)
        {
            return &SPC_3to2_load_;
        }
        else
        {
            return &SPC_3to2_coast_;
        }        
        break;

        case SPC_2to1:
        if (dtype == load)
        {
            return &SPC_2to1_load_;
        }
        else
        {
            return &SPC_2to1_coast_;
        }        
        break;

        default:
        return nullptr;
        break;
    }
}

// return a value from a given position
int configHandler::readDualAxisMap(dualAxisMap* map, int rowVal, int colVal)
{
    int i = 0;
    if (rowVal <= map->rowTitles[0]) // smaller or as small as the smallest in row
    { 
        i = 0;
    }
    else if(rowVal >= map->rowTitles[map->rows-1]) // bigger or as big as the biggest in row
    {
        i = map->rows-1;     
    }
    else // in between
    {
        for(i = 0; i < map->rows; i++)
        {
            if (rowVal <= map->rowTitles[i])
            {
                if (rowVal <  (map->rowTitles[i-1] + ((map->rowTitles[i] - map->rowTitles[i-1]) / 2))) // if smaller than halfway of two row titles, then use previous title
                {
                    i--;
                }
                break;
            }
        } 
    }

    int j = 0;
    if (colVal <= map->columnTitles[0]) // smaller or as small as the smallest in column
    { 
        j = 0;
    }
    else if(colVal >= map->columnTitles[map->columns-1]) // bigger or as big as the biggest in column
    {
        j = map->columns-1;     
    }
    else // in between
    {
        for(j = 0; j < map->columns; j++)
        {
            if (colVal <= map->columnTitles[j])
            {
                if (colVal <  (map->columnTitles[j-1] + ((map->columnTitles[j] - map->columnTitles[j-1]) / 2))) // if smaller than halfway of two column titles, then use previous title
                {
                    j--;
                }
                break;
            }
        }  
    }
    return map->data[i][j];  
}

// return map data as function of column titles
int configHandler::readSingleAxisMap(singleAxisMap* map, int colVal)
{
    if (colVal <= map->columnTitles[0]) // smaller or as small as the smallest in column
    { 
        return map->data[0];
    }
    else if(colVal >= map->columnTitles[map->columns-1]) // bigger or as big as the biggest in column
    {
        return map->data[map->columns-1];   
    }
    else // in between
    {
        for(int i = 0; i < map->columns; i++)
        {
            if (colVal <= map->columnTitles[i])
            {
                return linearlyInterpolate(map->columnTitles[i-1], map->columnTitles[i], colVal, map->data[i-1], map->data[i]);
            }
        }  
    }
    return 0;
}

// return column title as function of map data
int configHandler::readInverseSingleAxisMap(singleAxisMap* map, int colVal)
{
    if (colVal <= map->data[0]) // smaller or as small as the smallest in column
    { 
        return map->columnTitles[0];
    }
    else if(colVal >= map->data[map->columns-1]) // bigger or as big as the biggest in column
    {
        return map->columnTitles[map->columns-1];   
    }
    else // in between
    {
        for(int i = 0; i < map->columns; i++)
        {
            if (colVal <= map->data[i])
            {
                return linearlyInterpolate(map->data[i-1], map->data[i], colVal, map->columnTitles[i-1], map->columnTitles[i]);
            }
        }  
    }
    return 0;
}

int configHandler::linearizeBetweenTwoValues(int x1, int x2, int xVal, int y1, int y2) // THIS IS NOT NEEDED, SLOWER THAN WITH FLOATS!
{
    if (xVal <= x1 || y1 == y2)
    {
        return y1;
    }
    else if (xVal >= x2)
    {
        return y2;
    }
    else
    {
        int center = x1 + ((x2 - x1) * 0.5);
        if (xVal <= center)
        {
            return linearizeBetweenTwoValues(x1, center, xVal, y1, y1 + (y2 - y1) * 0.5);
        }
        else
        {
            return linearizeBetweenTwoValues(center, x2, xVal, y1 + (y2 - y1) * 0.5, y2);
        }
    }
}

int configHandler::linearlyInterpolate(float x1, float x2, float xVal, float y1, float y2)
{
    if (xVal < x1)
    {
        return y1;
    }
    else if (xVal > x2)
    {
        return y2;
    }
    else
    {
        return y1 + (((xVal - x1) / (x2 - x1)) * (y2 - y1));
    }
}

void configHandler::printDualAxisMapToSerial(shiftType stype, driveType dtype)
{
    dualAxisMap *map = identifyShiftMap(stype, dtype);

    Serial.println("RowTitles:");
    for(int i = 0; i < map->rows; i++)
    {
        Serial.print(map->rowTitles[i]);
        Serial.print(" ");
    }

        Serial.print("\n");
        Serial.println("ColumnTitles:");
    for(int i = 0; i <  map->columns; i++)
    {
        Serial.print(map->columnTitles[i]);
        Serial.print(" ");
    }

        Serial.print("\n");
        Serial.println("Data:");
    for(int i = 0; i <  map->rows; i++)
    {
        for (int j = 0; j <  map->columns; j++)
        {
            Serial.print(map->data[i][j]);
            Serial.print(" ");
        }
         Serial.print("\n");
    } 
}

// reads all
String configHandler::readSDCardToDataStructs(char filename[])
{  
    clearConfigReadFlags();
    if (!SD.begin(chipSelectPin_))
    {
        return "No SD card detected! No data read to RAM!";
    }
    // re-open the file for reading:
    myFile_ = SD.open(filename);
    if (myFile_)
    {
        // read from the file until there's nothing else in it:
        while (myFile_.available())
        {
        if (myFile_.read() == '#') // recognize new parameter
        {
            int i = 0;

            String currObjID;
            int dataCount = 0;
            int currObjData[200];

            while(i != ';') // start reading after hash, read until ; at end of object data
            {
            i = myFile_.read(); // read char

            if (i != 13 && i != 10) // 13 is ASCII for carriage return -> end of obj name
            {
                char c = i;
                currObjID.concat(c); // read name char by char to String
            }

            if (i == 10) // 10 is ASCII for new line, means that the actual data starts
            {
                int val = 0;
                bool negative;
                int minusSign = 45;
                while (true) // read the data and add it to array
                {
                i = myFile_.read();  
                if (i == minusSign) // if we have negative number
                {
                    negative = true;
                } 

                if (i == ',' || i == ';') // if we reach end of "section" (between periods, or with parameters end of line (;))
                {
                    if (negative)
                    {
                        val = val * -1; // inverse if needed
                    }
                    currObjData[dataCount] = val; // add value to array
                    dataCount++;
                    val = 0;
                    negative = false;
                }
                else if (i != minusSign) // do the conversion from chars to ints, ignore minus sign
                {
                    val = val * 10; // take decades into account
                    val = val + (i - '0');
                }

                    if (i == ';') // insert the current object data structures when reaching end of data
                    {
                        if (dataCount == 1) // if parameter
                        {
                            for (int mapLoc = 0; mapLoc < parametersAmount_; mapLoc++)
                            {
                                if (currObjID.equals(parameters_[mapLoc]->ID))
                                {
                                    parameters_[mapLoc]->data = currObjData[0]; // if parameter, only first of array to be set
                                    parameters_[mapLoc]->readFromSD = true; // set flag
                                }
                            }
                        }
                        else
                        {
                            if (currObjData[0] == 1) // if single axis map
                            {
                                //check if this exists in single axis maps
                                for (int mapLoc = 0; mapLoc < singleAxisMapsAmount_; mapLoc++)
                                {
                                    if (currObjID.equals(singleAxisMaps_[mapLoc]->ID))
                                    {
                                        singleAxisMaps_[mapLoc]->columns = currObjData[1];
                                        int counter = 2;
                                        for (int i = 0; i < currObjData[1]; i++) // add column titles
                                        {
                                            singleAxisMaps_[mapLoc]->columnTitles[i] = currObjData[counter];
                                            counter++;
                                        }
                                        for (int i = 0; i < currObjData[1]; i++) // add data
                                        {
                                            singleAxisMaps_[mapLoc]->data[i] = currObjData[counter];
                                            counter++;
                                        }
                                        singleAxisMaps_[mapLoc]->readFromSD = true; // set the flag
                                    }
                                }
                            }
                            else // if dual axis map
                            {
                                //check if this exists in dual axis maps
                                for (int mapLoc = 0; mapLoc < dualAxisMapsAmount_; mapLoc++)
                                {
                                    if (currObjID.equals(dualAxisMaps_[mapLoc]->ID))
                                    {
                                        dualAxisMaps_[mapLoc]->rows = currObjData[0];
                                        dualAxisMaps_[mapLoc]->columns = currObjData[1];

                                        int counter = 2;
                                        for (int i = 0; i < currObjData[0]; i++) // add row titles
                                        {
                                            dualAxisMaps_[mapLoc]->rowTitles[i] = currObjData[counter];
                                            counter++;
                                        }

                                        for (int i = 0; i < currObjData[1]; i++) // add column titles
                                        {
                                            dualAxisMaps_[mapLoc]->columnTitles[i] = currObjData[counter];
                                            counter++;
                                        }

                                        for (int i = 0; i < currObjData[0]; i++) // add data
                                        {
                                            for (int j = 0; j < currObjData[1]; j++)
                                            {
                                                dualAxisMaps_[mapLoc]->data[i][j] = currObjData[counter];
                                                counter++;
                                            }
                                        }
                                        dualAxisMaps_[mapLoc]->readFromSD = true; // set the flag
                                    }
                                }
                            }                    
                        }                
                        break; // exit the data section read loop
                    }
                }
            }
            
            }
        }      
        }
        // close the file and close connection to card
        myFile_.close();
        SD.end();
        //String msg = String("Config file ");
        //msg.concat(filename);
        //msg.concat(" read succesfully to RAM!");
        //return msg;
        if (checkConfigStatus())
        {
            String msg = String("Config file ");
            msg.concat(filename);
            msg.concat(" read succesfully to RAM!");
            return msg; 
        }
        else
        {
            String msg = String("Not all config was found from SD card! Fix the ");
            msg.concat(filename);
            msg.concat(" file!");
            return msg;   
        }
    }
    else
    {
        SD.end();
        // if the file didn't open
        String msg = String("Couldn't find config file ");
        msg.concat(filename);
        return msg;
    } 
}

String configHandler::writeDataStructsToSDCard(char filename[])
{
    if (!SD.begin(chipSelectPin_))
    {
        return "No SD card detected!";
    }
    SD.remove(filename); // remove old file first, otherwise data will be written into it after old lines
    myFile_ = SD.open(filename, FILE_WRITE); // open file, only one file can be open at once

    if (myFile_) // if the file opened, write to it
        {
        for (int i = 0; i < parametersAmount_; i++) // then single axis maps
        {
            if (parameters_[i]->readFromSD) // write only if the parameter is read from the card initially, as in there is sth to write back
            {
                if (i != 0) 
                {
                    myFile_.println(); // new line, but not at the beginning of file
                }
                    myFile_.print("#");
                    myFile_.println(parameters_[i]->ID); // note the line change
                    myFile_.print(parameters_[i]->data);
                    myFile_.print(";");
                    myFile_.println();
                }
        }

        for (int i = 0; i < singleAxisMapsAmount_; i++) // then single axis maps
        {
            if (singleAxisMaps_[i]->readFromSD) // write only if the map is read from the card initially, as in there is sth to write back
            {
                myFile_.println(); // new line
                myFile_.print("#");
                myFile_.println(singleAxisMaps_[i]->ID); // note the line change
                myFile_.print("1"); // 1 because of single axis
                myFile_.print(",");
                myFile_.print(singleAxisMaps_[i]->columns);
                myFile_.print(",");

                for (int j = 0; j < singleAxisMaps_[i]->columns; j++) // write column titles
                {
                    myFile_.print(singleAxisMaps_[i]->columnTitles[j]);
                    myFile_.print(",");
                }
                
                for (int j = 0; j < singleAxisMaps_[i]->columns; j++) // write data
                {
                    myFile_.print(singleAxisMaps_[i]->data[j]);
                    if (j != singleAxisMaps_[i]->columns-1) // add comma between the numbers
                    {
                        myFile_.print(",");
                    }
                    else // and semicolon after last one
                    {
                        myFile_.print(";");
                        myFile_.println(); // new line
                    }
                }
            }
        }        

        for (int i = 0; i < dualAxisMapsAmount_; i++) // then dual axis maps
        {
            if (dualAxisMaps_[i]->readFromSD) // write only if the map is read from the card initially, as in there is sth to write back
            {
                myFile_.println(); // new line
                myFile_.print("#");
                myFile_.println(dualAxisMaps_[i]->ID);
                myFile_.print(dualAxisMaps_[i]->rows);
                myFile_.print(",");
                myFile_.print(dualAxisMaps_[i]->columns);
                myFile_.print(",");

                for (int j = 0; j < dualAxisMaps_[i]->rows; j++) // write row titles
                {
                    myFile_.print(dualAxisMaps_[i]->rowTitles[j]);
                    myFile_.print(",");
                }

                for (int j = 0; j < dualAxisMaps_[i]->columns; j++) // write column titles
                {
                    myFile_.print(dualAxisMaps_[i]->columnTitles[j]);
                    myFile_.print(",");
                }

                for (int j = 0; j < dualAxisMaps_[i]->rows; j++) // write data
                {
                    for (int k = 0; k < dualAxisMaps_[i]->columns; k++)
                    {
                        myFile_.print(dualAxisMaps_[i]->data[j][k]);
                        if (j == dualAxisMaps_[i]->rows-1 && k == dualAxisMaps_[i]->columns-1)
                        {
                            myFile_.print(";");
                            myFile_.println();
                        }
                        else 
                        {
                            myFile_.print(","); 
                        }                      
                    }
                }
            }
        }
        myFile_.println();
        myFile_.print("EOF");

        // close the file:
        myFile_.close();
        SD.end();
        String msg = String("Config was succesfully written to ");
        msg.concat(filename);
        return msg;
        } 
    else
    {
        // if the file didn't open, print an error:
        String msg = String("Couldn't write to ");
        msg.concat(filename);
        return msg;
    }
}

void configHandler::clearConfigReadFlags()
{
    for(int i = 0; i < dualAxisMapsAmount_; i++)
    {
       dualAxisMaps_[i]->readFromSD = false;
    }
    for(int i = 0; i < singleAxisMapsAmount_; i++)
    {
       singleAxisMaps_[i]->readFromSD = false;
    }
    for(int i = 0; i < parametersAmount_; i++)
    {
       parameters_[i]->readFromSD = false;
    }
}

bool configHandler::checkConfigStatus()
{
    for(int i = 0; i < dualAxisMapsAmount_; i++)
    {
        if (!dualAxisMaps_[i]->readFromSD)
        {
            return false;
        } 
    }
    for(int i = 0; i < singleAxisMapsAmount_; i++)
    {
        if (!singleAxisMaps_[i]->readFromSD)
        {
            return false;
        } 
    }
    for(int i = 0; i < parametersAmount_; i++)
    {
        if (!parameters_[i]->readFromSD)
        {
            return false;
        } 
    }
    return true;
}

struct configHandler::configurationSet* configHandler::givePtrToConfigurationSet()
{
    return &configSet_;
}
