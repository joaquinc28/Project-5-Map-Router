#include "MapRouter.h"
#include <cmath>
#include <iostream>
const CMapRouter::TNodeID CMapRouter::InvalidNodeID = -1;

CMapRouter::CMapRouter(){

}

CMapRouter::~CMapRouter(){

}

double CMapRouter::HaversineDistance(double lat1, double lon1, double lat2, double lon2){
    auto DegreesToRadians = [](double deg){return M_PI * (deg) / 180.0;};
	double LatRad1 = DegreesToRadians(lat1);
	double LatRad2 = DegreesToRadians(lat2);
	double LonRad1 = DegreesToRadians(lon1);
	double LonRad2 = DegreesToRadians(lon2);
	double DeltaLat = LatRad2 - LatRad1;
	double DeltaLon = LonRad2 - LonRad1;
	double DeltaLatSin = sin(DeltaLat/2);
	double DeltaLonSin = sin(DeltaLon/2);
	double Computation = asin(sqrt(DeltaLatSin * DeltaLatSin + cos(LatRad1) * cos(LatRad2) * DeltaLonSin * DeltaLonSin));
	const double EarthRadiusMiles = 3959.88;

	return 2 * EarthRadiusMiles * Computation;
}

double CMapRouter::CalculateBearing(double lat1, double lon1,double lat2, double lon2){
    auto DegreesToRadians = [](double deg){return M_PI * (deg) / 180.0;};
    auto RadiansToDegrees = [](double rad){return 180.0 * (rad) / M_PI;};
    double LatRad1 = DegreesToRadians(lat1);
	double LatRad2 = DegreesToRadians(lat2);
	double LonRad1 = DegreesToRadians(lon1);
	double LonRad2 = DegreesToRadians(lon2);
    double X = cos(LatRad2)*sin(LonRad2-LonRad1);
    double Y = cos(LatRad1)*sin(LatRad2)-sin(LatRad1)*cos(LatRad2)*cos(LonRad2-LonRad1);
    return RadiansToDegrees(atan2(X,Y));
}

bool CMapRouter::LoadMapAndRoutes(std::istream &osm, std::istream &stops, std::istream &routes){
    CXMLReader reader(osm);
    SXMLEntity TempEnt;

    reader.ReadEntity(TempEnt);
    if(TempEnt.DType != SXMLEntity::EType::StartElement or TempEnt.DNameData != "osm"){
        return false;
    }
    while(!reader.End()){
	    reader.ReadEntity(TempEnt);
	    printf("Not logical value at line number %d \n",__LINE__);
	if(TempEnt.DType == SXMLEntity::EType::EndElement){
	    if(TempEnt.DNameData == "osm"){
		    printf("Not logical value at line number %d \n",__LINE__);
	         break;
	    }
	}
        if(TempEnt.DType == SXMLEntity::EType::StartElement){
		printf("Not logical value at line number %d \n",__LINE__);
            if(TempEnt.DNameData == "node"){
		    printf("Not logical value at line number %d \n",__LINE__);
                TNodeID  TempId = std::stoul(TempEnt.AttributeValue("id"));
                double TempLat = std::stoul(TempEnt.AttributeValue("lat"));
                double TempLon = std::stoul(TempEnt.AttributeValue("lon"));
                node TempNode;
                TempNode.nodeid = TempId;
                TempNode.location = std::make_pair(TempLat, TempLon);
                position.emplace(TempId,nodes.size());
                nodes.push_back(TempNode);
                SortedIds.push_back(TempId);
            }

            else if(TempEnt.DNameData == "way"){
                bool oneway = false;
                int speed_limit = 25;
                std::vector<TnodeIndex> wayorder;
                while(!reader.End()){
			printf("Not logical value at line number %d \n",__LINE__);
                    reader.ReadEntity(TempEnt);
                    if(TempEnt.DType == SXMLEntity::EType::EndElement){
                        if(TempEnt.DNameData == "way"){
                            break;
                        }
                    }
                    if(TempEnt.DType == SXMLEntity::EType::StartElement){
                        if(TempEnt.DNameData == "nd"){
                            auto lookup = position.find(std::stoul(TempEnt.AttributeValue("ref")));
                            if(lookup != position.end()){
                                wayorder.push_back(lookup->second);

                            }
                        }
                        if(TempEnt.DNameData == "tag"){
                            if(TempEnt.AttributeValue("k") == "oneway"){
                                if(TempEnt.AttributeValue("v") == "yes"){
                                    oneway = true;
                                }
                            }
                        }
                        if(TempEnt.AttributeValue("k") == "maxspeed"){
                            std::string speed = TempEnt.AttributeValue("v");
                            int npos = speed.find_first_of(" ");
                            std::string speed1 = speed.substr(0,npos);
                            speed_limit = std::stoul(speed1);

                        }
                    }
                }

                for(int i = 0; i < wayorder.size() - 1 ;i++){
                    edge tempedge;
                    tempedge.ConnectedNode = wayorder[i + 1];
                    tempedge.speed = speed_limit;
                    double distance = HaversineDistance(nodes[wayorder[i]].location.first,
                                                          nodes[wayorder[i]].location.second,
                                                          nodes[wayorder[i + 1]].location.first,
                                                          nodes[wayorder[i + 1]].location.second);
                    tempedge.distance = distance;
                    nodes[wayorder[i]].edges.push_back(tempedge);
                    if(!oneway) {
                        edge edge2;
                        edge2.ConnectedNode = wayorder[i];
                        edge2.speed = speed_limit;
                        edge2.distance = distance;
                        nodes[wayorder[i + 1]].edges.push_back(edge2);

                    }


                }

                }

            }
        }
    for(auto c:nodes){
        std::cout<<c.nodeid<<std::endl;
	std::cout<<c.location.first<<std::endl;
        std::cout<<c.location.second<<std::endl;

    }
    for(int i = 0; i < nodes.size() - 1; i++){
        if (nodes[i].edges.size() == 1) {
            std::cout<<nodes[i].edges[0].ConnectedNode<<std::endl;
        }
	if(nodes[i].edges.size() == 2){
            std::cout<<nodes[i].edges[1].ConnectedNode<<std::endl;
        }
        if(nodes[i].edges.size() == 3){
            std::cout<<nodes[i].edges[2].ConnectedNode<<std::endl;
        }
    }

return true;
}

size_t CMapRouter::NodeCount() const{
    // Your code HERE
    return nodes.size();
}

CMapRouter::TNodeID CMapRouter::GetSortedNodeIDByIndex(size_t index) const{
    // Your code HERE
    return nodes[index].nodeid;
}

CMapRouter::TLocation CMapRouter::GetSortedNodeLocationByIndex(size_t index) const{
    // Your code HERE
    return nodes[index].location;
}

CMapRouter::TLocation CMapRouter::GetNodeLocationByID(TNodeID nodeid) const{
    // Your code HERE
    int index = position.at(nodeid);
    return nodes[index].location;
}

CMapRouter::TNodeID CMapRouter::GetNodeIDByStopID(TStopID stopid) const{
    // Your code HERE
}

size_t CMapRouter::RouteCount() const{
    // Your code HERE
}

std::string CMapRouter::GetSortedRouteNameByIndex(size_t index) const{
    auto Search = RouteTranslations.find(route);
    if(RouteTranslations.end() != Search) {
        for (auto &NodeIndex : Search->second.StopIndices) {
            stops.push_back(NodeToStopTranslation.find(NodeIndex)->second);
        }
        return true;
    }
    return false;
    // Your code HERE
}

bool CMapRouter::GetRouteStopsByRouteName(const std::string &route, std::vector< TStopID > &stops){
    // Your code HERE
}

double CMapRouter::Dijkstra(TnodeIndex src, TnodeIndex dest, std::vector<TnodeIndex> &path, int searchtype){
    std::vector<TnodeIndex> Previous(Nodes.size()); //vector of Previous nodes
    std::vector<double> Distance(Nodes.size(), std::numeric_limits<double>::max()); //vector of distances to src node, initialized to infinity
    std::vector<TnodeIndex> Heap; //initialize heap as empty vector
    auto Compare = [&Distance] (TnodeIndex idx1, TnodeIndex idx2) {return Distance[idx1] < Distance[idx2];}; //returns true if dist idx1 < idx2
    Previous[src] = src; 
    Distance[src] = 0.0;
    Heap.push_back(src);

    while(!Heap.empty()) {
        std::make_heap(Heap.begin(), Heap.end(), Compare);
        std::pop_heap(Heap.begin(), Heap.end(), Compare);
        auto Current = Heap.back();
        Heap.pop_back();
        for(auto &Edge : Nodes[Current].Edges) { //iterate through neighboring edges of current node (popped from heap)
            double EdgeDistance;
            switch(searchtype) {
                case 0: EdgeDistance = Edge.Distance;
                        break;
                case 1: EdgeDistance = Edge.Time;
                        break;
                default EdgeDistance = Edge.Distance/Edge.SpeedLimit;
                        break;
            }
            auto AltDistance = Distance[Current] + EdgeDistance;
            if (AltDistance < Distance[Edge.ConnectedNode]) {
                if(Distance[Edge.ConnectedNide] == std::numeric_limit<double>::max()) {
                    Heap.push_back(Edge.ConnectedNode);
                }
                Distance[Edge.ConnectedNode] = AltDistance;
                Previous[Edge.ConnectedNode] = Current;
            }
        }
    }

    if(Distance[dest] == std::numeric_limits<double>::max()){
        return std::numeric_limits<double>::max();
    }
    else {
        return Distance[dest]; 
    }
}

double CMapRouter::FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path){
    auto srcIdx = position[src];
    auto destIdx = position[dest];
    std::vector<TnodeID> path;
    double distance = Dijkstra(sourceIdx, destIdx, path, 0);
    
    return distance;
    // Your code HERE
}

double CMapRouter::FindFastestPath(TNodeID src, TNodeID dest, std::vector< TPathStep > &path){
    auto srcIdx = position[src];
    auto destIdx = position[dest];
    std::vector<TnodeID> path;
    double distance = Dijkstra(sourceIdx, destIdx, path, searchtype);
    // Your code HERE
}

bool CMapRouter::GetPathDescription(const std::vector< TPathStep > &path, std::vector< std::string > &desc) const{
    // Your code HERE
}
