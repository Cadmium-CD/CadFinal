
#include "RGReader.h"


using namespace std;


RouteGuide addRouteGuide(DefParser::Net net, double x0, double y0, double x1, double y1){
    
    RouteGuide rg = RouteGuide(x0,y0,x1,y1);
    rg.addNet(net);
    return rg;
    
}

RouteGuide* searchRouteGuide(vector<RouteGuide> vRouteGuide,double x0, double y0, double x1, double y1){
    for (int i = 0; i < vRouteGuide.size(); ++i) {
        
        RouteGuide rg = vRouteGuide[i];
        
        if ((rg.x0 == x0) && (rg.y0 == y0) && (rg.x1 == x1) && (rg.y1 == y1)){
            return &rg;
        }
    }
    
    return nullptr;
}

// net stored in route guides
vector<vector<RouteGuide>> parseRouteGuid(string fileName, RawDataBase r_db){
    
    
    std::ifstream fin(fileName);
    
    DefParser::Net currentNet;
    //RouteGuide currentRouteGuide;
    vector<vector<RouteGuide>> vRouteGuide(9);
    
    // helper var for parsing
    bool isNet = true;
    bool firstParen = false;
    
    std::string currentStr;
    
    std::string netName;
    double x0,y0, x1, y1;
    int numMetal;
    std::string metal;
    
    //for each line in route guide file
    for (std::string line; std::getline(fin, line); ) {
        
        // removing punctuation like paranthesis, commas
        //std::replace_if(std::begin(line), std::end(line), [] (char x) { return std::ispunct(x); }, ' ');
        
        // inserting the line into a stream that helps us parse the content
        std::stringstream ss;
        ss<<line;
        
        ss >> currentStr;
        //cout<<currentStr[0]<<endl;
        
        if (isNet){
            
            isNet = false;
            
            // search for current net
            stringstream stream(currentStr);
            stream >> netName;
            currentNet = r_db.getNet(netName);
            //cout <<currentNet<<endl;
            
        } else if ((currentStr[0] == 0x28) | (currentStr[0] == 0x29) ){
                //cout<<"hey"<<endl;
            if (firstParen == true) {
                isNet = true;
                firstParen = false;
                //cout<<"true"<<endl;
            } else {
                firstParen = true;
            }
            
        } else {
            //parse line to get route guide info
            stringstream stream(currentStr);
            stream >> x0;
            ss >> y0 >> x1 >> y1 >> metal;
            numMetal = (int) metal[5] - 48;
            
            //search if the route guide is already in list
            RouteGuide* rgPtr = searchRouteGuide(vRouteGuide[numMetal-1],x0,y0,x1,y1);
            if (!rgPtr) {
                RouteGuide rg = addRouteGuide(currentNet,x0,y0,x1,y1);
                //cout<<rg.netArray.size()<<endl;
                vRouteGuide[numMetal-1].push_back(rg);
            } else {
                rgPtr->addNet(currentNet);
                //cout<<rgPtr->netArray.size()<<endl;
            }
        }
    }
    //cout<<vRouteGuide[0].size()<<endl;
    return vRouteGuide;
}

//find all potential coordinates


//creat Panel
vector<Metal> processRG(vector<vector<RouteGuide>> vRG,map<int, int> vertMap,map<int, int> horizMap){
    vector<Metal> vMetal(9);
    for(int i = 0;i<9;i++){
        vMetal[i].setHoriz((i+1)%2);
    }
    for(int i = 0;i<9;i++){
        vMetal[i].seIndex(i+1);
    }
    for(int i = 0;i<9;i++){
        vMetal[i].creatPanel(vRG[i],vertMap,horizMap);
        //cout<<"creat"<<endl;
    }

    return vMetal;
}

//generate APC between pinshape and metal
genPinShapeAPC( vector<Metal>* iMetal, MacroDataBase* macro_db,RawDataBase _db){
    //metal tempMetal = (*iMetal)[0];
    for(int i = 0;i<(*iMetal)[0].panelArray.size();i++){
            //current routeguide 
            for(iter = (*iMetal)[0].panelArray[i].rgList.begin(); iter != (*iMetal)[0].panelArray[j].rgList.end(); iter++) {
                //current net/ should have a APC
                for(;it != iter->second.netArray.end(); ++it){
                        APC currentAPC.net = *it;
                        //the net has 1.inst1123 A 2. inst 1134 B ...... f
                        for (uint32_t j = 0; j < it->vNetPin.size(); ++j){
                            //find the information for current inst
                            DefParser::Component currentComp = _db.findComponentInfo(it->vNetPin.first);
                            //check if the current inst is belonge to this guide
                            if(checkInstPosition(currentComp,iter->second)){
                                StdCell currentStd = macro_db.getStdCellLib(currentComp.macro_name);
                                //pinA pinB etc
                                Pin currentPin = currentStd.findPin(it->vNetPin.second);
                                //find the position of intersection and place it in to a ap class
                                for(k = 0;k< currentPin.recArray.size(),k++){
                                    vector<AP> tempAp = findGenAP((*iMetal)[0].panelArray[i],iter->second,currentStb,currentComp,currentPin.recArray[k]);
                                     for(;itap != tempAp.end(); ++itap){
                                         currentAPC.addAP(*itap);
                                     }  
                                }
                            }

                        }
                   //pushback apc to current routeguide
                    iter->second.addNetAPC(currentAPC); 
                 }
            }   

    }
}

void findOverlap(vector<Metal>* iMetal,map<int, int> vertMap,map<int, int> horizMap){
    for(int i = 0; i<(*iMetal).size();i++){
        if((*iMetal)[i].horiz){
            for(int j = 0; j<(*iMetal)[i].panelArray.size();j++){
                //cout<<(*iMetal)[i].panelArray[j].rgList.size()<<endl;
                for(int k = 0;k<horizMap.size();k++){
                //upper
                if(i<8 and (*iMetal)[i+1].panelArray[j].rgList.find(k) != (*iMetal)[i+1].panelArray[j].rgList.end()){
                    //cout<<"overlap"<<endl;
                    (*iMetal)[i].panelArray[j].upperOverlap.push_back(true); 
                    //cout<<vMetal[i].panelArray[j].upperOverlap[k]<<endl;
                }else{
                    (*iMetal)[i].panelArray[j].upperOverlap.push_back(false);
                }
                //lower
                if(i>0 and (*iMetal)[i-1].panelArray[j].rgList.find(k) != (*iMetal)[i-1].panelArray[j].rgList.end()){
                    (*iMetal)[i].panelArray[j].lowerOverlap.push_back(true);
                }else{
                    (*iMetal)[i].panelArray[j].lowerOverlap.push_back(false);
                }
                }   
            }
        }else{
            for(int j = 0; j<(*iMetal)[i].panelArray.size();j++){
                //cout<<(*iMetal)[i].panelArray[j].rgList.size()<<endl;
                for(int k = 0;k<vertMap.size();k++){
                //upper
                if(i<8 and (*iMetal)[i+1].panelArray[j].rgList.find(k) != (*iMetal)[i+1].panelArray[j].rgList.end()){
                    //cout<<"overlap"<<endl;
                    (*iMetal)[i].panelArray[j].upperOverlap.push_back(true); 
                    //cout<<vMetal[i].panelArray[j].upperOverlap[k]<<endl;
                }else{
                    (*iMetal)[i].panelArray[j].upperOverlap.push_back(false);
                }
                //lower
                if(i>0 and (*iMetal)[i-1].panelArray[j].rgList.find(k) != (*iMetal)[i-1].panelArray[j].rgList.end()){
                    (*iMetal)[i].panelArray[j].lowerOverlap.push_back(true);
                }else{
                    (*iMetal)[i].panelArray[j].lowerOverlap.push_back(false);
                }
                }   
            }
        }
        

    }
    cout<<(*iMetal)[1].panelArray[1].lowerOverlap[1]<<endl;
    //cout<<vMetal[2].panelArray[1].upperOverlap[1]<<endl;
}

void generateGrid(vector<Metal>* iMetal){
        int unitH = 380;
        int unitW = 400;
        int numX = 52;
        int numY = 51;
        int startX = 83800;
        int stattY = 72010;

    for(int i = 0; i<(*iMetal).size();i++){
        if((*iMetal)[i].horiz){//metal1 380
            int tempH = 0;
            int tempW = 0;
            for(int j = 0; j<(*iMetal)[i].panelArray.size();j++){
                for(int k = 0;k<(*iMetal)[i].panelArray[j].fixedWidth/unitH;k++){
                    for(int x = 0;x<numX;numX++){
                        (*iMetal)[i].panelArray[j].gridArray[k][x].xg = startX+x*unitW;
                        (*iMetal)[i].panelArray[j].gridArray[k][x].yg = startY+k*unitH;
                    }
                    startY = startY + (*iMetal)[i].panelArray[j].fixedWidth;
                }   
            }
        }else{//metal2 400
            int tempH = 0;
            int tempW = 0;
            for(int j = 0; j<(*iMetal)[i].panelArray.size();j++){
                for(int k = 0;k<(*iMetal)[i].panelArray[j].fixedWidth/unitW;k++){
                    for(int x = 0;x<numY;numY++){
                        (*iMetal)[i].panelArray[j].gridArray[k][x].xg = k*unitW;
                        (*iMetal)[i].panelArray[j].gridArray[k][x].yg = x*unitH;
                    }
                    startY = startY + (*iMetal)[i].panelArray[j].fixedWidth;
                }   
            }
        }
        

    }
}
//check whether the pin is in the routeguide
int checkInstPosition(DefParser::Component comp, RouteGuide rg){
    if((comp.origin[0]>rg.x0) && (comp.origin[0]<rg.x1) && (comp.origin[1]>rg.y0) && (comp.origin[1]<rg.y1)){
        return 1;
    }else
        return 0;
}
vector<AP> findGenAP(panel panel,RouteGuide rg,StdCell cell,DefParser::Component comp,Recunit rec){
    vector<AP> vAP;
    int unit = 2000;
    //origin of cell should be align weith component origin but we have not store cell origin, they seems to be zero
    int originx = comp.origin[0];
    int originx = comp.origin[1];
    int sizex = cell.sizeX*unit;
    int sizey = cell.sizeY*unit;
    switch(comp.orient)
   {
        case 'N' :
                int recx0 = originx + rec.x0*unit;
                int recx1 = originx + rec.x1*unit;
                int recy0 = originy + rec.y0*unit;
                int recy1 = originy + rec.y1*unit;
            break;
        case 'S' :
                int recx0 = originx + sizex - rec.x0*unit;
                int recx1 = originx + sizex - rec.x1*unit;
                int recy0 = originy + sizey - rec.y0*unit;
                int recy1 = originy + sizey - rec.y1*unit;
            break;  
        case 'W' :
                int recx0 = originx + sizey - rec.y1*unit;
                int recx1 = originx + sizey - rec.y0*unit;
                int recy0 = originy + rec.x0*unit;
                int recy1 = originy + rec.x1*unit;
            break;
        case 'E' :
                int recx0 = originx + rec.y0*unit;
                int recx1 = originx + rec.y1*unit;
                int recy0 = originy + sizeY - rec.x1*unit;
                int recy1 = originy + sizeY - rec.x0*unit;
            break;
        case 'FN' :
                int recx0 = originx - rec.x0*unit;
                int recx1 = originx - rec.x1*unit;
                int recy0 = originy + rec.y0*unit;
                int recy1 = originy + rec.y1*unit;
            break;
        case 'FS' :
                int recx0 = originx - sizex + rec.x0*unit;
                int recx1 = originx - sizex + rec.x1*unit;
                int recy0 = originy + sizey - rec.y0*unit;
                int recy1 = originy + sizey - rec.y1*unit;
            break;
        case 'FW' :
                int recx0 = originx - sizey + rec.y1*unit;
                int recx1 = originx - sizey + rec.y0*unit;
                int recy0 = originy + rec.x0*unit;
                int recy1 = originy + rec.x1*unit;
            break;
        case 'FE' :
                int recx0 = originx - rec.y0*unit;
                int recx1 = originx - rec.y1*unit;
                int recy0 = originy + sizeY - rec.x1*unit;
                int recy1 = originy + sizeY - rec.x0*unit;
            break;
   }

    for(i = 0; i < panel.gridArray.size();i++ ){
        for(j = 0; j< panel.gridArray[i].size(),j++){
            tempx = panel.gridArray[i][j].xg;
            tempy = panel.gridArray[i][j].yg;
            if((xg>recx0)&&(xg<recx1)&&(yg>recy0)&&(yg<recy1)){
                AP tempap.x = tempx;
                AP tempap.y = tempy;
                vAP.push_back();
            }
        }
    }
    return vAP;
}
