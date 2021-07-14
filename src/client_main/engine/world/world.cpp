#include <fstream>

#include "world.h"
#include "engine/entities/character.h"
#include "engine/entities/character_appearance_manager.h"
#include "engine/game.h"
#include "utils/util.h"
#include "api/logging/logging.h"

namespace projectfarm::engine::world
{
    bool World::Load(const std::string& name)
    {
        this->_plots->SetDataProvider(this->_dataProvider);
        this->_plots->SetTileSetPool(this->_tileSetPool);
        if (!this->_plots->Load(name))
        {
            shared::api::logging::Log("Failed to load plots.");
            return false;
        }

        auto locations = this->_dataProvider->GetWorldLocations();

        auto fileNameIter = std::find_if(locations.begin(), locations.end(), [&name](const auto& it)
        {
            return it.first == name;
        });

        if (fileNameIter == locations.end())
        {
            shared::api::logging::Log("Failed to find world location with name: " + name);
            return false;
        }

        std::filesystem::path filePath = fileNameIter->second;

        return this->Load(filePath);
    }

    bool World::Load(const std::filesystem::path& filePath)
    {
        if (filePath.extension() == ".bin")
        {
            return this->LoadFromBinaryFile(filePath);
        }
        else
        {
            return this->LoadFromJsonFile(filePath);
        }
    }

    bool World::LoadFromBinaryFile(const std::filesystem::path& filePath)
    {
        std::ifstream fs(filePath, std::ios::binary);

        if (!fs.is_open())
        {
            shared::api::logging::Log("Failed to open world file: " + filePath.u8string());
            return false;
        }

        this->_name = pfu::ReadStringFromBinaryFile(fs);

        auto island = std::make_shared<Island>();
        island->SetLogger(this->_logger);
        island->SetDataProvider(this->_dataProvider);
        island->SetGraphics(this->GetGraphics());
        island->SetRenderManager(this->_renderManager);
        island->SetTileSetPool(this->_tileSetPool);
        island->SetTimer(this->GetTimer());

        if (!island->LoadFromBinary(fs, this->_plots))
        {
            shared::api::logging::Log("Failed to load island.");
            return false;
        }

        this->_islands.emplace_back(std::move(island));

        return true;
    }

    bool World::LoadFromJsonFile(const std::filesystem::path& filePath)
    {
        std::ifstream file(filePath);

        if (!file.is_open())
        {
            shared::api::logging::Log("Failed to open world file: " + filePath.u8string());
            return false;
        }

        nlohmann::json jsonFile;
        file >> jsonFile;

        this->_name = jsonFile["name"].get<std::string>();

        auto islandsJson = jsonFile["islands"];

        for (const auto& islandJson : islandsJson)
        {
            auto island = std::make_shared<Island>();
            island->SetLogger(this->_logger);
            island->SetDataProvider(this->_dataProvider);
            island->SetGraphics(this->GetGraphics());
            island->SetRenderManager(this->_renderManager);
            island->SetTileSetPool(this->_tileSetPool);
            island->SetTimer(this->GetTimer());

            if (!island->LoadFromJson(islandJson, this->_plots))
            {
                shared::api::logging::Log("Failed to load island.");
                return false;
            }

            this->_islands.emplace_back(std::move(island));
        }

        auto jsonIt = jsonFile.find("background");
        if (jsonIt != jsonFile.end())
        {
            if (!this->LoadBackground(*jsonIt))
            {
                shared::api::logging::Log("Failed to load background.");
                return false;
            }
        }

        return true;
    }

    void World::Shutdown()
    {
        for (auto& entity : this->_entities)
        {
            entity->Shutdown();
        }

        this->_entities.clear();

        for (auto& island : this->_islands)
        {
            island->Shutdown();
        }

        this->_islands.clear();

        this->_plots->Shutdown();
    }

    void World::Tick()
    {
        if (this->_backgroundIsland != nullptr)
        {
            this->_backgroundIsland->Tick();
        }

        for (auto& island : this->_islands)
        {
            island->Tick();
        }

        this->UpdateEntities();

        this->UpdateIslandRenderBounds();
    }

    void World::Render() noexcept
    {
        if (this->_backgroundIsland)
        {
            auto numberOfLayersInBackgroundIsland = 0u;

            auto islandWidth = this->_backgroundIsland->GetWidthInPixels();
            auto islandHeight = this->_backgroundIsland->GetHeightInPixels();

            auto viewportW = this->GetGraphics()->GetCamera()->GetViewport().w;
            auto viewportH = this->GetGraphics()->GetCamera()->GetViewport().h;

            for (auto x = 0u; x < viewportW + islandWidth; x += islandWidth)
            {
                for (auto y = 0u; y < viewportH + islandHeight; y += islandHeight)
                {
                    // we want to duplicate these islands on the same render layers,
                    // so save and restore the layers respectively
                    auto renderLayer = this->GetGraphics()->GetRenderLayer();

                    this->_backgroundIsland->SetRenderDetails(x, y);
                    this->_backgroundIsland->Render();

                    numberOfLayersInBackgroundIsland =
                            this->GetGraphics()->GetRenderLayer() - renderLayer;

                    this->GetGraphics()->SetRenderLayer(renderLayer);
                }
            }

            // move the render layer back forward
            this->GetGraphics()->SetRenderLayer(
                    this->GetGraphics()->GetRenderLayer() + numberOfLayersInBackgroundIsland);
        }

        for (const auto& island : this->_islands)
        {
            island->RenderGround();
        }

        for (const auto& entity : this->_entities)
        {
            entity->Render();
        }

        for (const auto& island : this->_islands)
        {
            island->RenderOverhead();
        }
    }

    bool World::LoadBackground(const nlohmann::json& backgroundJson)
    {
        auto type = backgroundJson["type"].get<std::string>();

        if (type == "island")
        {
            auto islandJson = backgroundJson["island"];
            if (!this->LoadBackgroundIsland(islandJson))
            {
                shared::api::logging::Log("Failed to load background island.");
                return false;
            }
        }

        return true;
    }

    bool World::LoadBackgroundIsland(const nlohmann::json& islandJson)
    {
        this->_backgroundIsland = std::make_shared<Island>();
        this->_backgroundIsland->SetLogger(this->_logger);
        this->_backgroundIsland->SetDataProvider(this->_dataProvider);
        this->_backgroundIsland->SetGraphics(this->GetGraphics());
        this->_backgroundIsland->SetRenderManager(this->_renderManager);
        this->_backgroundIsland->SetTileSetPool(this->_tileSetPool);
        this->_backgroundIsland->SetTimer(this->GetTimer());

        this->_backgroundIsland->SetRenderToWorldSpace(false);

        if (!this->_backgroundIsland->LoadFromJson(islandJson, this->_plots))
        {
            shared::api::logging::Log("Failed to load island.");
            return false;
        }

        this->_backgroundIsland->SetRenderDetails(0, 0,
                                                  this->_backgroundIsland->GetWidthInPixels(),
                                                  this->_backgroundIsland->GetHeightInPixels(),
                                                  graphics::RenderOriginPoints::TopLeft);

        return true;
    }

    void World::UpdateEntities()
    {
        for (auto& entity : this->_entities)
        {
            entity->Tick();

            if (entity->ShouldBroadcastState())
            {
                this->GetScene()->GetSceneManager()->BroadcastEntityState(entity,
                        this->GetTimer()->GetTotalGameDurationInMicroseconds());
            }
        }
    }

    bool World::UpdateEntity(uint32_t entityId, uint32_t playerId, shared::entities::EntityTypes entityType,
                             uint64_t lastUpdateTime, const std::vector<std::byte>& data) noexcept
    {
        auto entityIt = std::find_if(this->_entities.begin(), this->_entities.end(),
                [entityId](const auto& it)
                {
                    return it->GetEntityId() == entityId;
                });

        std::shared_ptr<entities::Entity> entity;

        if (entityIt == this->_entities.end())
        {
            if (entity = this->CreateNewEntity(entityId, playerId, entityType); !entity)
            {
                shared::api::logging::Log("Failed to create entity of type: " +
                    std::to_string(static_cast<int>(entityType)));

                return false;
            }
        }
        else
        {
            entity = *entityIt;
        }

        if (entity->GetLastUpdateTime() >= lastUpdateTime)
        {
            return true;
        }

        entity->SetLastUpdateTime(lastUpdateTime);
        entity->SetData(data);

        return true;
    }

    std::shared_ptr<entities::Entity> World::CreateNewEntity(uint32_t entityId, uint32_t playerId,
            shared::entities::EntityTypes entityType) noexcept
    {
        std::shared_ptr<entities::Entity> entity;

        switch (entityType)
        {
            case shared::entities::EntityTypes::Unknown:
            {
                shared::api::logging::Log("Cannot create entity of type: Unknown");
                return nullptr;
            }
            case shared::entities::EntityTypes::World:
            {
                shared::api::logging::Log("Cannot create entity of type: World");
                return nullptr;
            }
            case shared::entities::EntityTypes::Character:
            {
                auto character = std::make_shared<entities::Character>();
                character->SetGraphics(this->GetGraphics());
                character->SetDataProvider(this->_dataProvider);
                character->SetRenderManager(this->GetRenderManager());
                character->SetTileSetPool(this->GetTileSetPool());
                character->SetGame(this->GetGame());

                character->SetCurrentWorld(this->shared_from_this());

                entity = std::dynamic_pointer_cast<entities::Entity>(character);
                break;
            }
        }

        if (entity == nullptr)
        {
            shared::api::logging::Log("Unknown entity type: " +
                std::to_string(static_cast<int>(entityType)));

            return nullptr;
        }

        entity->SetLogger(this->_logger);
        entity->SetTimer(this->GetTimer());
        entity->SetEntityId(entityId);

        if (!entity->Load())
        {
            shared::api::logging::Log("Failed to load entity with type: " +
                             std::to_string(static_cast<int>(entityType)));

            return nullptr;
        }

        auto& player = this->GetGame()->GetPlayer();
        if (playerId == player.GetPlayerId())
        {
            if (auto character = std::dynamic_pointer_cast<entities::Character>(entity);
                character && !player.GetCharacter())
            {
                player.SetCharacter(character);

                this->GetScene()->OnPlayerCharacterAdd(character);

                character->SetBroadcastStateTime(50); // TODO: Set time by config or script
                character->SetShouldBroadcastState(true);
            }
        }

        shared::api::logging::Log("Created new entity");

        this->_entities.emplace_back(entity);

        return entity;
    }

    void World::RemoveEntity(uint32_t entityId) noexcept
    {
        auto entityIt = std::find_if(this->_entities.begin(), this->_entities.end(),
            [entityId](const auto& e) { return e->GetEntityId() == entityId; });

        if (entityIt == this->_entities.end())
        {
            shared::api::logging::Log("Failed to find entity with id: " + std::to_string(entityId) + " to remove it.");
            return;
        }

        (*entityIt)->Shutdown();

        this->_entities.remove_if([entityId](const auto& e) { return e->GetEntityId() == entityId; });
    }

    void World::ReconfirmPixelSizes() noexcept
    {
        if (this->_backgroundIsland)
        {
            this->_backgroundIsland->ReconfirmPixelSizes();
        }

        for (const auto& island : this->_islands)
        {
            island->ReconfirmPixelSizes();
        }

        for (const auto& entity : this->_entities)
        {
            entity->ReconfirmPixelSizes();
        }
    }

    void World::UpdateIslandRenderBounds() noexcept
    {
        const auto& viewport = this->GetGraphics()->GetCamera()->GetViewport();

        for (auto& island : this->_islands)
        {
            island->SetRenderBounds(viewport);
        }
    }

    uint16_t World::GetPlotIndexFromWorldPosition(float x, float y) const noexcept
    {
        for (const auto& island : this->_islands)
        {
            auto index = island->GetPlotIndexAtWorldPosition(x, y);
            if (index != Plots::EmptyIndex)
            {
                return index;
            }
        }

        return Plots::EmptyIndex;
    }

    std::pair<int32_t, int32_t> World::GetTileIndexesFromWorldPosition(float x, float y) const noexcept
    {
        for (const auto& island : this->_islands)
        {
            auto [tx, ty] = island->GetTileIndexesFromWorldPosition(x, y);
            if (tx != -1 && ty != -1)
            {
                return {tx, ty};
            }
        }

        return {-1, -1};
    }

    std::optional<std::shared_ptr<entities::Character>> World::GetCharacterByEntityId(uint32_t entityId) const noexcept
    {
        auto entity = std::find_if(this->_entities.begin(), this->_entities.end(),
                [entityId](const auto& e) { return e->GetEntityId() == entityId; });

        if (entity == this->_entities.end())
        {
            return {};
        }

        if (auto character = std::dynamic_pointer_cast<entities::Character>(*entity); character)
        {
            return character;
        }

        return {};
    }
}
