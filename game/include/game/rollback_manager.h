#pragma once
#include "bullet_manager.h"
#include "game_globals.h"
#include "physics_manager.h"
#include "player_character.h"
#include "engine/entity.h"
#include "engine/transform.h"
#include "network/packet_type.h"



namespace game
{
class GameManager;

    struct CreatedEntity
    {
        core::Entity entity = core::EntityManager::INVALID_ENTITY;
        Frame createdFrame = 0;
    };

    struct DestroyedBullet
    {
        Bullet bullet;
        Body body;
        Frame destroyedFrame = 0;
    };

    class RollbackManager : public OnTriggerInterface
    {
    public:
        explicit RollbackManager(GameManager& gameManager, core::EntityManager& entityManager);
        /**
         * \brief Simulate all players with new inputs, method call only by the clients
         */
        void SimulateToCurrentFrame();
        void SetPlayerInput(PlayerNumber playerNumber, PlayerInput playerInput, Frame inputFrame);
        void StartNewFrame(Frame newFrame);
        /**
         * \brief Validate all the frame from lastValidateFrame_ to newValidateFrame
         */
        void ValidateFrame(Frame newValidateFrame);
        /**
         * \brief Confirm Frame and Check with Physics State checksum, called by the clients when receiving Confirm Frame packet
         */
        void ConfirmFrame(Frame newValidatedFrame, const std::array<PhysicsState, maxPlayerNmb>& serverPhysicsState);
        [[nodiscard]] PhysicsState GetValidatePhysicsState(PlayerNumber playerNumber) const;
        [[nodiscard]] Frame GetLastValidateFrame() const { return lastValidateFrame_; }
        [[nodiscard]] Frame GetLastReceivedFrame(PlayerNumber playerNumber) const { return lastReceivedFrame_[playerNumber]; }
        [[nodiscard]] Frame GetCurrentFrame() const { return currentFrame_; }
        [[nodiscard]] const core::TransformManager& GetTransformManager() const { return currentTransformManager_; }
        [[nodiscard]] const PlayerCharacterManager& GetPlayerCharacterManager() const { return currentPlayerManager_; }
        void SpawnPlayer(PlayerNumber playerNumber, core::Entity entity, core::Vec2f position, core::degree_t rotation);
        void SpawnBullet(PlayerNumber playerNumber, core::Entity entity, core::Vec2f position, core::Vec2f velocity);
        /**
         * \brief This function does not destroy the entity definitely, but puts the DESTROY flag
         */
        void DestroyEntity(core::Entity entity);

        void OnTrigger(core::Entity entity1, core::Entity entity2) override;
    private:
        PlayerInput GetInputAtFrame(PlayerNumber playerNumber, Frame frame);
        GameManager& gameManager_;
        core::EntityManager& entityManager_;
        /**
         * \brief Used for rendering
         */
        core::TransformManager currentTransformManager_;
        PhysicsManager currentPhysicsManager_;
        PlayerCharacterManager currentPlayerManager_;
        BulletManager currentBulletManager_;
        /**
         * Last Validate (confirm frame) Component Managers used for rollback
         */
        PhysicsManager lastValidatePhysicsManager_;
        PlayerCharacterManager lastValidatePlayerManager_;
        BulletManager lastValidateBulletManager_;


        Frame lastValidateFrame_ = 0; //Confirm frame
        Frame currentFrame_ = 0;
        Frame testedFrame_ = 0;

        static constexpr std::size_t windowBufferSize = 5 * 50; // 5 seconds of frame at 50 fps
        std::array<std::uint32_t, maxPlayerNmb> lastReceivedFrame_{};
        std::array<std::array<PlayerInput, windowBufferSize>, maxPlayerNmb> inputs_{};
        /**
         * \brief Array containing all the created entities in the window between the confirm frame and the current frame
         * to destroy them when rollbacking.
         */
        std::vector<CreatedEntity> createdEntities_;
    public:
        [[nodiscard]] const std::array<PlayerInput, windowBufferSize>& GetInputs(PlayerNumber playerNumber) const
        {
            return inputs_[playerNumber];
        }

    };
}
