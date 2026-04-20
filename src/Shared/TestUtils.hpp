#pragma once

#include "Server/Server.h"
#include "Client/Client.h"
#include "Client/Client.h"
#include "Network/Message.h"
#include "Logging/Logger.h"
#include "Logging/ConsoleLog.h"
#include "Shared/Messages.h"
#include <cassert>
#include <atomic>
#include <thread>
#include <array>

class TestServer
{
    private:
    std::atomic<bool> _running;
    std::thread _worker;

    public:
    TestServer()
        : _running (true)
        , _worker ([&](){ 
            Server server;
            while(server.HandleEvents() || _running) Logger::GetInstance().Trace("[Test Server] Running");
            Logger::GetInstance().Trace("[Test Server] Finished running");
        }) 
    { }

    ~TestServer() 
    {
        _running = false;
        _worker.join();
    }
};

void SetTestLog()
{
    Logger::GetInstance().SetTarget(std::make_unique<ConsoleLog>());
    Logger::GetInstance().SetLevel(Logger::Level::Trace);
}

void AssertResponse(Client& c, Message&& request, std::size_t response_type)
{
    auto response = c.SendRequest(request);
    assert(response.has_value());
    assert(response.value().index() == response_type);
}

std::array<std::string, 100> usernames {
    "alice_adams",
    "brian_baker",
    "chloe_carter",
    "daniel_dixon",
    "emma_evans",
    "felix_fisher",
    "grace_gomez",
    "harry_hayes",
    "isabel_irwin",
    "jacob_jones",
    "kevin_klein",
    "laura_lewis",
    "megan_moore",
    "nolan_nelson",
    "olivia_owens",
    "peter_perez",
    "quinn_quincy",
    "ryan_reyes",
    "sarah_smith",
    "tyler_taylor",
    "uma_upton",
    "victor_vance",
    "wendy_white",
    "xavier_xiong",
    "yara_yates",
    "zack_zeller",
    "aaron_allen",
    "bella_brooks",
    "caleb_clark",
    "diana_davis",
    "ethan_ellis",
    "fiona_ford",
    "gavin_green",
    "haley_harris",
    "ian_ingram",
    "julia_james",
    "kyle_kelly",
    "liam_lopez",
    "mason_martin",
    "nora_nixon",
    "oscar_ortiz",
    "paige_price",
    "quincy_quinn",
    "riley_reed",
    "sophia_scott",
    "thomas_tran",
    "uriel_usher",
    "vera_vargas",
    "wyatt_wood",
    "xena_xiang",
    "yusuf_yusuf",
    "zoe_zuniga",
    "adam_austin",
    "blake_bailey",
    "clara_cruz",
    "dylan_diaz",
    "elena_edwards",
    "finn_foster",
    "gemma_garcia",
    "hugo_howard",
    "iris_ibarra",
    "jace_jackson",
    "kara_king",
    "lucas_lee",
    "mia_miller",
    "noah_nelson",
    "owen_ochoa",
    "piper_perry",
    "quinn_quarles",
    "roman_ross",
    "stella_sims",
    "theo_thomas",
    "ursula_urbina",
    "vance_vega",
    "willa_ward",
    "xander_ximenes",
    "yvonne_yoder",
    "zane_zimmerman",
    "amelia_abbott",
    "brody_benson",
    "cora_collins",
    "declan_dawson",
    "elliott_ellis",
    "flora_fox",
    "grant_gibson",
    "hazel_hale",
    "isaac_ives",
    "jade_jones",
    "knox_knight",
    "luna_lawson",
    "milo_myers",
    "nova_nash",
    "oliver_ortega",
    "penny_patel",
    "quigley_quigley",
    "rowan_rowe",
    "silas_stone",
    "tessa_tucker",
    "ulric_unger",
    "violet_vaughn"
};

std::array<std::string, 100> passwords {
    "aliceAdams123!",
    "brianBaker234@",
    "chloeCarter345#",
    "danielDixon456$",
    "emmaEvans567%",
    "felixFisher678^",
    "graceGomez789&",
    "harryHayes890*",
    "isabelIrwin901-",
    "jacobJones012+",
    "kevinKlein111=",
    "lauraLewis222?",
    "meganMoore333!",
    "nolanNelson444@",
    "oliviaOwens555#",
    "peterPerez666$",
    "quinnQuincy777%",
    "ryanReyes888^",
    "sarahSmith999&",
    "tylerTaylor101*",
    "umaUpton202-",
    "victorVance303+",
    "wendyWhite404=",
    "xavierXiong505?",
    "yaraYates606!",
    "zackZeller707@",
    "aaronAllen808#",
    "bellaBrooks909$",
    "calebClark121%",
    "dianaDavis232^",
    "ethanEllis343&",
    "fionaFord454*",
    "gavinGreen565-",
    "haleyHarris676+",
    "ianIngram787=",
    "juliaJames898?",
    "kyleKelly909!",
    "liamLopez010@",
    "masonMartin135#",
    "noraNixon246$",
    "oscarOrtiz357%",
    "paigePrice468^",
    "quincyQuinn579&",
    "rileyReed680*",
    "sophiaScott791-",
    "thomasTran802+",
    "urielUsher913=",
    "veraVargas024?",
    "wyattWood147!",
    "xenaXiang258@",
    "yusufYusuf369#",
    "zoeZuniga470$",
    "adamAustin581%",
    "blakeBailey692^",
    "claraCruz703&",
    "dylanDiaz814*",
    "elenaEdwards925-",
    "finnFoster036+",
    "gemmaGarcia147=",
    "hugoHoward258?",
    "irisIbarra369!",
    "jaceJackson470@",
    "karaKing581#",
    "lucasLee692$",
    "miaMiller703%",
    "noahNelson814^",
    "owenOchoa925&",
    "piperPerry036*",
    "quinnQuarles159-",
    "romanRoss260+",
    "stellaSims371=",
    "theoThomas482?",
    "ursulaUrbina593!",
    "vanceVega604@",
    "willaWard715#",
    "xanderXimenes826$",
    "yvonneYoder937%",
    "zaneZimmerman048^",
    "ameliaAbbott159&",
    "brodyBenson260*",
    "coraCollins371-",
    "declanDawson482+",
    "elliottEllis593=",
    "floraFox604?",
    "grantGibson715!",
    "hazelHale826@",
    "isaacIves937#",
    "jadeJones048$",
    "knoxKnight160%",
    "lunaLawson271^",
    "miloMyers382&",
    "novaNash493*",
    "oliverOrtega504-",
    "pennyPatel615+",
    "quigleyQuigley726=",
    "rowanRowe837?",
    "silasStone948!",
    "tessaTucker059@",
    "ulricUnger160#",
    "violetVaughn271$"
};

std::string accounts(int i)
{
    return usernames[i] + " " + passwords[i];
}