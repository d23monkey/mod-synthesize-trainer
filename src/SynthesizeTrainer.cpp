#include "ScriptMgr.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "Chat.h"
#include "SpellMgr.h"
#include "WorldSession.h"
#include "Pet.h"

class CreatureScript_SynthesizeTrainer : public CreatureScript
{
public:
    CreatureScript_SynthesizeTrainer() : CreatureScript("synthesize_trainer") {}
	
	
    bool OnGossipHello(Player *player, Creature *creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Achievement_BG_trueAVshutout:25|t职业技能", 1, 1);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_Tailoring:25|t商业技能", 1, 2);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Mount_Dreadsteed:25|t骑术技能", 1, 3);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_DualWield:25|t武器技能", 1, 4);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Holy_DevineAegis:25|t开双天赋", 1, 5, "你确定要开双天赋吗？", 0, false);
		AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Arcane_MindMastery:25|t重置天赋", 1, 6, "你确定要重置天赋吗？", 0, false);
		AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Hunter_SeparationAnxiety:25|t重置宠物天赋", 1, 7, "你确定要重置宠物天赋吗？", 0, false);
		AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Rune_01:25|t炉石绑定", 1, 8);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature);
        return true;
    }

    bool OnGossipSelect(Player *player, Creature *creature, uint32 Sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (Sender == 1)
        {
            if (action == 1)
            {
                uint32 npcspellid = 0;
                switch (player->getClass())
                {
                    case CLASS_WARRIOR: npcspellid = 3042; break;
                    case CLASS_PALADIN:   player->GetTeamId() == TEAM_HORDE ? npcspellid = 16679 : npcspellid = 16761; break;
                    case CLASS_HUNTER:    npcspellid = 3352; break;
                    case CLASS_ROGUE:    npcspellid = 3328; break;
                    case CLASS_PRIEST:    npcspellid = 6014; break;
                    case CLASS_DEATH_KNIGHT:    npcspellid = 28472; break;
                    case CLASS_SHAMAN:    player->GetTeamId() == TEAM_HORDE ? npcspellid = 3344 : npcspellid = 17219; break;
                    case CLASS_MAGE:    npcspellid = 16652; break;
                    case CLASS_WARLOCK:    npcspellid = 988; break;
                    case CLASS_DRUID:    npcspellid = 3036; break;
                }
                SendTrainerList(player, creature->GetGUID(), npcspellid);
                CloseGossipMenuFor(player);
            }
            else if (action == 2)
            {
                learnPSkill(player, creature); 
            }
            else if (action == 3)
            {
                SendTrainerList(player, creature->GetGUID(), 31238);
                CloseGossipMenuFor(player);
            }
            else if (action == 4)
            {
                SendTrainerList(player, creature->GetGUID(), 190017);
                CloseGossipMenuFor(player);
            }
            else if (action == 5)
            {
                // Cast spells that teach dual spec
                // Both are also ImplicitTarget self and must be cast by player
                if (player->GetSpecsCount() > 1)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("你已经学会了双天赋，不能重复学习！");
                }
                else if (player->GetLevel() < sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL))
                {
                    ChatHandler(player->GetSession()).PSendSysMessage(" 你的等级还不足{}级，还不能开启双天赋!", sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL));
                }
                else
                {
                    player->CastSpell(player, 63680, true, NULL, NULL, player->GetGUID());
                    player->CastSpell(player, 63624, true, NULL, NULL, player->GetGUID());

                    ChatHandler(player->GetSession()).PSendSysMessage("你成功学会了双天赋!");
                };
                player->PlayerTalkClass->SendCloseGossip();
            }
			else if (action == 6)
            {
                uint8 specPoints[3] = {0, 0, 0};
                player->GetTalentTreePoints(specPoints);
                uint8 totalPoints = specPoints[0] + specPoints[1] + specPoints[2];

				//printf("Talent points in trees: %u, %u, %u\n", specPoints[0], specPoints[1], specPoints[2]);
				//printf("Total talent points: %u\n", totalPoints);

                if (totalPoints > 0)//判断是否点选了天赋
                {
                    player->resetTalents(true);
                    player->SendTalentsInfoData(false);
                    player->CastSpell(player, 14867, true);
                    ChatHandler(player->GetSession()).PSendSysMessage("你的天赋已被重置！");
                }
                else
                {
					ChatHandler(player->GetSession()).PSendSysMessage("你还没有分配天赋点！");
                }
                CloseGossipMenuFor(player);
            }
            else if (action == 7)
            {
                if (player->getClass() == CLASS_HUNTER && player->GetPet())
                {
                    Pet* pet = player->GetPet(); // 获取玩家的宠物
                    if (pet && pet->m_usedTalentCount > 0)//判断宠物是否点了天赋
                    {
                        player->ResetPetTalents();
                        ChatHandler(player->GetSession()).PSendSysMessage("你的宠物天赋已被重置！");
                    }
                    else
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("你的宠物没有分配天赋点，无需重置。");
                    }
                }
                else
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("你还没有宠物！");
                }
                CloseGossipMenuFor(player);
            }
            else if (action == 8)
            {
				player->SetBindPoint(creature->GetGUID());
                player->PlayerTalkClass->SendCloseGossip();
            }
        }
        else if (Sender == 2) {
            uint32 npcskillid = 0; //技能训练师ID
            switch (action)
            {
                case 1:     npcskillid = 28698; break; //采矿
                case 2:     npcskillid = 28704; break; //草药
                case 3:     npcskillid = 28703; break; //炼金
                case 4:     npcskillid = 28693; break; //附魔
                case 5:     npcskillid = 28694; break; //锻造
                case 6:     npcskillid = 28699; break; //裁缝
                case 7:     npcskillid = 28697; break; //工程
                case 8:     npcskillid = 28701; break; //珠宝
                case 9:     npcskillid = 28700; break; //制皮
                case 10:    npcskillid = 28696; break; //剥皮
                case 11:    npcskillid = 28702; break; //铭文
                case 12:    npcskillid = 28705; break; //烹饪
                case 13:    npcskillid = 28742; break; //钓鱼
                case 14:    npcskillid = 28706; break; //急救
            }
            SendTrainerList(player, creature->GetGUID(), npcskillid);
            CloseGossipMenuFor(player);
        }
        return true;
    }
   //专业训练菜单
    static void learnPSkill(Player* player, Creature* creature)
    {
        player->PlayerTalkClass->ClearMenus();
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "采矿训练", 2, 1);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "草药训练", 2, 2);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "炼金训练", 2, 3);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "附魔训练", 2, 4);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "锻造训练", 2, 5);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "裁缝训练", 2, 6);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "工程训练", 2, 7);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "珠宝训练", 2, 8);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "制皮训练", 2, 9);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "剥皮训练", 2, 10);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "铭文训练", 2, 11);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "烹饪训练", 2, 12);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "钓鱼训练", 2, 13);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "急救训练", 2, 14);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature);
    }
private:
    static void SendTrainerList(Player* player, ObjectGuid guid, uint32 npcspellid)
    {
        WorldSession* session = player->GetSession();
        if (!session) return;
        std::string strTitle = session->GetAcoreString(LANG_NPC_TAINER_HELLO);

        LOG_DEBUG("network", "WORLD: SendTrainerList");
        Creature* unit = player->GetMap()->GetCreature(guid);

        if (!unit)
        {
            LOG_DEBUG("network", "WORLD: SendTrainerList - Unit ({}) not found or you can not interact with him.", guid.ToString());
            return;
        }

        // remove fake death
        if (player->HasUnitState(UNIT_STATE_DIED))
            player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        CreatureTemplate const* ci = unit->GetCreatureTemplate();

        if (!ci)
        {
            LOG_DEBUG("network", "WORLD: SendTrainerList - ({}) NO CREATUREINFO!", guid.ToString());
            return;
        }
        TrainerSpellData const* trainer_spells =  sObjectMgr->GetNpcTrainerSpells(npcspellid);

        if (!trainer_spells)
        {
            LOG_DEBUG("network", "WORLD: SendTrainerList - Training spells not found for creature ({})", guid.ToString());
            return;
        }

        WorldPacket data(SMSG_TRAINER_LIST, 8 + 4 + 4 + trainer_spells->spellList.size() * 38 + strTitle.size() + 1);
        data << guid;
        data << uint32(trainer_spells->trainerType);

        size_t count_pos = data.wpos();
        data << uint32(trainer_spells->spellList.size());

        // reputation discount
        float fDiscountMod = player->GetReputationPriceDiscount(unit);
        bool can_learn_primary_prof = player->GetFreePrimaryProfessionPoints() > 0;

        uint32 count = 0;
        for (TrainerSpellMap::const_iterator itr = trainer_spells->spellList.begin(); itr != trainer_spells->spellList.end(); ++itr)
        {
            TrainerSpell const* tSpell = &itr->second;

            bool valid = true;
            bool primary_prof_first_rank = false;
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (!tSpell->learnedSpell[i])
                    continue;
                if (!player->IsSpellFitByClassAndRace(tSpell->learnedSpell[i]))
                {
                    valid = false;
                    break;
                }
                SpellInfo const* learnedSpellInfo = sSpellMgr->GetSpellInfo(tSpell->learnedSpell[i]);
                if (learnedSpellInfo && learnedSpellInfo->IsPrimaryProfessionFirstRank())
                    primary_prof_first_rank = true;
            }

            if (!valid)
                continue;

            if (tSpell->reqSpell && !player->HasSpell(tSpell->reqSpell))
            {
                continue;
            }

            TrainerSpellState state = player->GetTrainerSpellState(tSpell);

            data << uint32(tSpell->spell);                      // learned spell (or cast-spell in profession case)
            data << uint8(state == TRAINER_SPELL_GREEN_DISABLED ? TRAINER_SPELL_GREEN : state);
            data << uint32(floor(tSpell->spellCost * fDiscountMod));

            data << uint32(primary_prof_first_rank && can_learn_primary_prof ? 1 : 0);
            // primary prof. learn confirmation dialog
            data << uint32(primary_prof_first_rank ? 1 : 0);    // must be equal prev. field to have learn button in enabled state
            data << uint8(tSpell->reqLevel);
            data << uint32(tSpell->reqSkill);
            data << uint32(tSpell->reqSkillValue);
            //prev + req or req + 0
            uint8 maxReq = 0;
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (!tSpell->learnedSpell[i])
                    continue;
                if (uint32 prevSpellId = sSpellMgr->GetPrevSpellInChain(tSpell->learnedSpell[i]))
                {
                    data << uint32(prevSpellId);
                    ++maxReq;
                }
                if (maxReq == 3)
                    break;
                SpellsRequiringSpellMapBounds spellsRequired = sSpellMgr->GetSpellsRequiredForSpellBounds(tSpell->learnedSpell[i]);
                for (SpellsRequiringSpellMap::const_iterator itr2 = spellsRequired.first; itr2 != spellsRequired.second && maxReq < 3; ++itr2)
                {
                    data << uint32(itr2->second);
                    ++maxReq;
                }
                if (maxReq == 3)
                    break;
            }
            while (maxReq < 3)
            {
                data << uint32(0);
                ++maxReq;
            }

            ++count;
        }

        data << strTitle;

        data.put<uint32>(count_pos, count);

        session->SendPacket(&data);
    }
};

void AddSC_SynthesizeTrainer()
{
    new CreatureScript_SynthesizeTrainer();
}
