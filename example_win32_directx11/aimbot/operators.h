#pragma once

#include "operators_icons.h"
#include <fstream>
#include <string>
#include <cstring>

struct RecoilPattern {
    const char* name;
    const char* weapon;
    float vertical;
    float horizontal;
    float burst_comp;
};

inline RecoilPattern recoil_data[] = {
    {"Ace", "AK-12", 0.42f, 0.11f, 0.1f},
    {"Alibi", "MX4 Storm", 0.28f, 0.07f, 0.1f},
    {"Aruni", "P10 Roni", 0.22f, 0.05f, 0.1f},
    {"Ash", "R4-C", 1.25f, 0.0f, 0.15f},
    {"Azami", "9x19VSN", 0.28f, 0.07f, 0.1f},
    {"Bandit", "MP7", 0.3f, 0.08f, 0.1f},
    {"Blackbeard", "MK17", 0.22f, 0.04f, 0.1f},
    {"Brava", "CAMR", 0.22f, 0.04f, 0.1f},
    {"Buck", "C8-SFW", 0.38f, 0.1f, 0.1f},
    {"Castle", "UMP45", 0.22f, 0.04f, 0.1f},
    {"Caveira", "M12", 0.24f, 0.05f, 0.1f},
    {"Deimos", "AK-74M", 0.32f, 0.08f, 0.1f},
    {"Doc", "MP5", 0.26f, 0.06f, 0.1f},
    {"Dokkaebi", "DMR", 0.2f, 0.03f, 0.1f},
    {"Echo", "MP5", 0.26f, 0.06f, 0.1f},
    {"Ela", "Scorpion", 0.48f, 0.18f, 0.1f},
    {"Fenrir", "MP5", 0.26f, 0.06f, 0.1f},
    {"Finka", "Spear .308", 0.35f, 0.08f, 0.1f},
    {"Flores", "AR33", 0.32f, 0.09f, 0.1f},
    {"Frost", "9mm C1", 0.24f, 0.05f, 0.1f},
    {"Fuze", "AK-12", 0.42f, 0.11f, 0.1f},
    {"Glaz", "OTs-03", 0.25f, 0.05f, 0.1f},
    {"Goyo", "Vector .45", 0.32f, 0.12f, 0.1f},
    {"Gridlock", "F90", 0.34f, 0.09f, 0.1f},
    {"Grim", "552 Commando", 0.36f, 0.09f, 0.1f},
    {"Hibana", "Type-89", 0.38f, 0.12f, 0.1f},
    {"IQ", "552 Commando", 0.36f, 0.09f, 0.1f},
    {"Jackal", "C7E", 0.34f, 0.08f, 0.1f},
    {"Jager", "416-C", 0.38f, 0.1f, 0.1f},
    {"Kaid", "TCSG12", 0.2f, 0.03f, 0.1f},
    {"Kapkan", "9x19VSN", 0.28f, 0.07f, 0.1f},
    {"Lesion", "T-5 SMG", 0.28f, 0.07f, 0.1f},
    {"Lion", "V308", 0.38f, 0.1f, 0.1f},
    {"Maestro", "ALDA 5.56", 0.4f, 0.1f, 0.1f},
    {"Maverick", "AR-15.50", 0.18f, 0.03f, 0.1f},
    {"Melusi", "MP5", 0.26f, 0.06f, 0.1f},
    {"Mira", "Vector .45", 0.32f, 0.12f, 0.1f},
    {"Mozzie", "P10 Roni", 0.22f, 0.05f, 0.1f},
    {"Mute", "MP5K", 0.26f, 0.06f, 0.1f},
    {"Nomad", "AK-74M", 0.32f, 0.08f, 0.1f},
    {"Oryx", "MP5", 0.26f, 0.06f, 0.1f},
    {"Osa", "556XI", 0.4f, 0.1f, 0.1f},
    {"Pulse", "UMP45", 0.22f, 0.04f, 0.1f},
    {"Ram", "R4-C", 0.35f, 0.08f, 0.1f},
    {"Rook", "MP5", 0.26f, 0.06f, 0.1f},
    {"Sens", "R4-0", 0.36f, 0.1f, 0.1f},
    {"Sledge", "L85A2", 0.3f, 0.06f, 0.1f},
    {"Smoke", "SMG-11", 0.42f, 0.15f, 0.1f},
    {"Solis", "P10 Roni", 0.22f, 0.05f, 0.1f},
    {"Tachanka", "DP-27", 0.45f, 0.12f, 0.1f},
    {"Thatcher", "AR33", 0.32f, 0.09f, 0.1f},
    {"Thermite", "556XI", 0.4f, 0.1f, 0.1f},
    {"Thorn", "UZK50GI", 0.34f, 0.09f, 0.1f},
    {"Thunderbird", "Spear .308", 0.35f, 0.08f, 0.1f},
    {"Tubarao", "AR-15.50", 0.18f, 0.03f, 0.1f},
    {"Twitch", "F2", 0.28f, 0.07f, 0.1f},
    {"Valkyrie", "MPX", 0.22f, 0.04f, 0.1f},
    {"Vigil", "K1A", 0.3f, 0.08f, 0.1f},
    {"Wamai", "AUG A2", 0.32f, 0.08f, 0.1f},
    {"Warden", "MP5", 0.26f, 0.06f, 0.1f},
    {"Ying", "T-95", 0.36f, 0.09f, 0.1f},
    {"Zero", "SC3000K", 0.3f, 0.07f, 0.1f},
    {"Zofia", "M762", 0.4f, 0.11f, 0.1f},
};
inline int recoil_data_count = 63;

inline void loadRecoilPatterns() {
    char exePath[MAX_PATH] = { 0 };
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::string path(exePath);
    size_t lastSlash = path.find_last_of("\\/");
    if (lastSlash != std::string::npos)
        path = path.substr(0, lastSlash + 1);
    path += "recoil_patterns.txt";

    std::ifstream f(path);
    if (!f.is_open()) return;

    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t pos = 0;
        auto next = [&]() -> std::string {
            size_t end = line.find(',', pos);
            std::string token = (end != std::string::npos) ? line.substr(pos, end - pos) : line.substr(pos);
            pos = (end != std::string::npos) ? end + 1 : line.size();
            return token;
        };

        std::string name = next();
        std::string weapon = next();
        std::string sv = next();
        std::string sh = next();
        std::string sb = next();

        for (int i = 0; i < recoil_data_count; i++) {
            if (_stricmp(recoil_data[i].name, name.c_str()) == 0) {
                if (!sv.empty()) recoil_data[i].vertical = (float)atof(sv.c_str());
                if (!sh.empty()) recoil_data[i].horizontal = (float)atof(sh.c_str());
                if (!sb.empty()) recoil_data[i].burst_comp = (float)atof(sb.c_str());
                break;
            }
        }
    }
}

inline const char* operator_ids[] = {
    "ace",
    "alibi",
    "aruni",
    "ash",
    "azami",
    "bandit",
    "blackbeard",
    "brava",
    "buck",
    "castle",
    "caveira",
    "deimos",
    "doc",
    "dokkaebi",
    "echo",
    "ela",
    "fenrir",
    "finka",
    "flores",
    "frost",
    "fuze",
    "glaz",
    "goyo",
    "gridlock",
    "grim",
    "hibana",
    "iq",
    "jackal",
    "jager",
    "kaid",
    "kapkan",
    "lesion",
    "lion",
    "maestro",
    "maverick",
    "melusi",
    "mira",
    "mozzie",
    "mute",
    "nomad",
    "oryx",
    "osa",
    "pulse",
    "ram",
    "rook",
    "sens",
    "sledge",
    "smoke",
    "solis",
    "tachanka",
    "thatcher",
    "thermite",
    "thorn",
    "thunderbird",
    "tubarao",
    "twitch",
    "valkyrie",
    "vigil",
    "wamai",
    "warden",
    "ying",
    "zero",
    "zofia",
};

struct OperatorIcon {
    unsigned char* data;
    int size;
};

inline bool is_atk[] = {
    true,  // ace
    false, // alibi
    false, // aruni
    true,  // ash
    false, // azami
    false, // bandit
    true,  // blackbeard
    true,  // brava
    true,  // buck
    false, // castle
    false, // caveira
    true,  // deimos
    false, // doc
    true,  // dokkaebi
    false, // echo
    false, // ela
    false, // fenrir
    true,  // finka
    true,  // flores
    false, // frost
    true,  // fuze
    true,  // glaz
    false, // goyo
    true,  // gridlock
    true,  // grim
    true,  // hibana
    true,  // iq
    true,  // jackal
    false, // jager
    false, // kaid
    false, // kapkan
    false, // lesion
    true,  // lion
    false, // maestro
    true,  // maverick
    false, // melusi
    false, // mira
    false, // mozzie
    false, // mute
    true,  // nomad
    false, // oryx
    true,  // osa
    false, // pulse
    true,  // ram
    false, // rook
    true,  // sens
    true,  // sledge
    false, // smoke
    false, // solis
    false, // tachanka
    true,  // thatcher
    true,  // thermite
    false, // thorn
    false, // thunderbird
    false, // tubarao
    true,  // twitch
    false, // valkyrie
    false, // vigil
    false, // wamai
    false, // warden
    true,  // ying
    true,  // zero
    true,  // zofia
};

inline OperatorIcon operator_icons[] = {
    {icon_ace, icon_ace_size},
    {icon_alibi, icon_alibi_size},
    {icon_aruni, icon_aruni_size},
    {icon_ash, icon_ash_size},
    {icon_azami, icon_azami_size},
    {icon_bandit, icon_bandit_size},
    {icon_blackbeard, icon_blackbeard_size},
    {icon_brava, icon_brava_size},
    {icon_buck, icon_buck_size},
    {icon_castle, icon_castle_size},
    {icon_caveira, icon_caveira_size},
    {icon_deimos, icon_deimos_size},
    {icon_doc, icon_doc_size},
    {icon_dokkaebi, icon_dokkaebi_size},
    {icon_echo, icon_echo_size},
    {icon_ela, icon_ela_size},
    {icon_fenrir, icon_fenrir_size},
    {icon_finka, icon_finka_size},
    {icon_flores, icon_flores_size},
    {icon_frost, icon_frost_size},
    {icon_fuze, icon_fuze_size},
    {icon_glaz, icon_glaz_size},
    {icon_goyo, icon_goyo_size},
    {icon_gridlock, icon_gridlock_size},
    {icon_grim, icon_grim_size},
    {icon_hibana, icon_hibana_size},
    {icon_iq, icon_iq_size},
    {icon_jackal, icon_jackal_size},
    {icon_jager, icon_jager_size},
    {icon_kaid, icon_kaid_size},
    {icon_kapkan, icon_kapkan_size},
    {icon_lesion, icon_lesion_size},
    {icon_lion, icon_lion_size},
    {icon_maestro, icon_maestro_size},
    {icon_maverick, icon_maverick_size},
    {icon_melusi, icon_melusi_size},
    {icon_mira, icon_mira_size},
    {icon_mozzie, icon_mozzie_size},
    {icon_mute, icon_mute_size},
    {icon_nomad, icon_nomad_size},
    {icon_oryx, icon_oryx_size},
    {icon_osa, icon_osa_size},
    {icon_pulse, icon_pulse_size},
    {icon_ram, icon_ram_size},
    {icon_rook, icon_rook_size},
    {icon_sens, icon_sens_size},
    {icon_sledge, icon_sledge_size},
    {icon_smoke, icon_smoke_size},
    {icon_solis, icon_solis_size},
    {icon_tachanka, icon_tachanka_size},
    {icon_thatcher, icon_thatcher_size},
    {icon_thermite, icon_thermite_size},
    {icon_thorn, icon_thorn_size},
    {icon_thunderbird, icon_thunderbird_size},
    {icon_tubarao, icon_tubarao_size},
    {icon_twitch, icon_twitch_size},
    {icon_valkyrie, icon_valkyrie_size},
    {icon_vigil, icon_vigil_size},
    {icon_wamai, icon_wamai_size},
    {icon_warden, icon_warden_size},
    {icon_ying, icon_ying_size},
    {icon_zero, icon_zero_size},
    {icon_zofia, icon_zofia_size},
};