#include "METUMaps.h"
#include "GraphExceptions.h"

#include <iostream>

void METUMaps::PrintNotInJourney() const
{
    std::cout << "Device is not in a journey!" << std::endl;
}

void METUMaps::PrintUnableToChangeDestination() const
{
    std::cout << "Cannot change Destination during journey!" << std::endl;
}

void METUMaps::PrintUnableToChangeStartingLoc() const
{
    std::cout << "Cannot change Starting Location during journey!" << std::endl;
}

void METUMaps::PrintAlreadyInJourney() const
{
    std::cout << "Device is already in a journey!" << std::endl;
}

void METUMaps::PrintJourneyIsAlreadFinished() const
{
    std::cout << "Journey is already finished!" << std::endl;
}

void METUMaps::PrintLocationNotFound() const
{
    std::cout << "One (or both) of the locations are not found in the maps!" << std::endl;
}

void METUMaps::PrintJourneyCompleted() const
{
    std::cout << "Journey Completed!" << std::endl;
}

void METUMaps::PrintCachedLocationFound(const std::string& location0,
                                        const std::string& location1) const
{
    std::cout << "Route between \""
              << location0 << "\" and \""
              << location1 << "\" is in cache, using that..."
              << std::endl;
}

void METUMaps::PrintCalculatingRoutes(const std::string& location0,
                                      const std::string& location1) const
{
    std::cout << "Calculating Route(s) between \""
              << location0 << "\" and \""
              << location1 << "\"..."
              << std::endl;
}

std::string METUMaps::GenerateKey(const std::string& location0,
                                  const std::string& location1)
{
    return location0 + "/" + location1;
}

METUMaps::METUMaps(int potentialPathCount,
                   const std::string& mapFilePath)
{
    Graph g(mapFilePath);
    map = g;
    this->potentialPathCount = potentialPathCount;
    inJourney = false;
    KeyedHashTable h(map.TotalVertexCount() * potentialPathCount);
    cachedPaths = h;
}

void METUMaps::SetDestination(const std::string& name)
{
    if (inJourney) 
        PrintUnableToChangeDestination();
    else    
        destination = name;
}

void METUMaps::SetStartingLocation(const std::string& name)
{
    if (inJourney) 
        PrintUnableToChangeStartingLoc();
    else    
        startingLoc = name;
}

void METUMaps::StartJourney()
{
    if (inJourney) {
        PrintAlreadyInJourney();
        return;
    }
    PrintCalculatingRoutes(startingLoc, destination);
    std::vector<std::vector<int> > paths;
    try {
        map.MultipleShortPaths(paths, startingLoc, destination, potentialPathCount);
    }
    catch (VertexNotFoundException) {
        PrintLocationNotFound();
        return;
    }
    for (int i=0; i<paths.size(); i++) {
        for (int j=0; j<paths[i].size()-1; j++) {
            int d = paths[i].size()-1;
            std::vector<int> spath(paths[i].size()-j);
            for (int k=j; k<paths[i].size(); k++)
                spath[k-j] = paths[i][k];
            cachedPaths.Insert(GenerateKey(map.VertexName(spath[0]), map.VertexName(paths[i][d])), spath);
        }
    }
    inJourney = true;
    currentLoc =  startingLoc;
    currentRoute = paths[0];
}

void METUMaps::EndJourney()
{
    if (!inJourney) {
        PrintJourneyIsAlreadFinished();
        return;
    }
    inJourney = false;
    cachedPaths.ClearTable();
    destination = "";
    startingLoc = "";
    currentLoc = "";
}

void METUMaps::UpdateLocation(const std::string& name)
{
    if (!inJourney) {
        PrintNotInJourney();
        return;
    }
    if (name == destination) {
        PrintJourneyCompleted();
        return;
    }
    std::vector<int> path;
    if (cachedPaths.Find(path, GenerateKey(name, destination))) {
        PrintCachedLocationFound(name, destination);
        currentRoute = path;
        currentLoc = name;
    }
    else {
        PrintCalculatingRoutes(name, destination);
        try {
            map.ShortestPath(path, name, destination);
        }
        catch (VertexNotFoundException) {
            PrintLocationNotFound();
            return;
        }
        for (int j=0; j<path.size()-1; j++) {
            int d = path.size()-1;
            std::vector<int> spath(path.size()-j);
            for (int k=j; k<path.size(); k++)
                spath[k-j] = path[k];
            cachedPaths.Insert(GenerateKey(map.VertexName(spath[0]), map.VertexName(path[d])), spath);
        }
        currentRoute = path;
        currentLoc = name;
    }
}

void METUMaps::Display()
{
    if(!inJourney)
    {
        // We are not in journey, this function
        // shouldn't be called
        PrintNotInJourney();
        return;
    }

    int timeLeft = map.TotalWeightInBetween(currentRoute);

    // Print last known location
    std::cout << "Journey         : " << startingLoc << "->"
              << destination << "\n";
    std::cout << "Current Location: " << currentLoc << "\n";
    std::cout << "Time Left       : " << timeLeft << " minutes\n";
    std::cout << "Current Route   : ";
    map.PrintPath(currentRoute, true);

    std::cout.flush();
}