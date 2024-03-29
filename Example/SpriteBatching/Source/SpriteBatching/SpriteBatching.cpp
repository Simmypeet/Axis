﻿/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/Core>
#include <Axis/Graphics>
#include <Axis/Renderer>
#include <Axis/System>
#include <Axis/Window>

using namespace Axis;
using namespace Axis::System;
using namespace Axis::Window;
using namespace Axis::Graphics;
using namespace Axis::Core;
using namespace Axis::Renderer;

int main(int argc, char** argv)
{
    // Represents a single sprite on the screen
    struct Sprite final
    {
    public:
        // Default constructor
        Sprite() noexcept {}

        // Constructor
        Sprite(const SharedPointer<DisplayWindow>& window,
               const SharedPointer<ITextureView>&  texture,
               const Vector2F&                     initialPos,
               const Vector2F&                     direction,
               Float32                             speed,
               Float32                             rotationSpeed) noexcept :
            _window(window),
            _texture(texture),
            _position(initialPos),
            _direction(direction),
            _speed(speed),
            _rotationSpeed(rotationSpeed) {}

        // Updates sprite's position and direction
        void Update(const TimePeriod& deltaTime) noexcept
        {
            // Moves the sprite's position
            _position.X += (Float32)(_direction.X * _speed * deltaTime.GetTotalSeconds());
            _position.Y += (Float32)(_direction.Y * _speed * deltaTime.GetTotalSeconds());
            _rotation += (Float32)(_rotationSpeed * deltaTime.GetTotalSeconds());

            Vector2UI windowSize = _window->GetSize();

            // Makes sure that the sprite doesn't go out of the screen
            if (_position.X < 0 && _direction.X < 0)
                _direction.X *= -1;

            if (_position.Y < 0 && _direction.Y < 0)
                _direction.Y *= -1;

            if (_position.X > windowSize.X && _direction.X > 0)
                _direction.X *= -1;

            if (_position.Y > windowSize.Y && _direction.Y > 0)
                _direction.Y *= -1;
        }

        // Draws the sprite
        void Draw(SpriteBatch& spriteBatch) noexcept
        {
            constexpr Float32 ScalingSize = 3;

            // Draws the sprite to the screen
            spriteBatch.Draw(_texture,
                             _position,
                             RectangleI(0, 0, _texture->Description.ViewTexture->Description.Size.X, _texture->Description.ViewTexture->Description.Size.Y),
                             {1.0f, 1.0f, 1.0f, 1.0f},
                             (Float32)Math::ToRadians(_rotation),
                             {_texture->Description.ViewTexture->Description.Size.X / 2.0f, _texture->Description.ViewTexture->Description.Size.Y / 2.0f},
                             {ScalingSize, ScalingSize},
                             SpriteEffect::None,
                             0.0f);
        }

    private:
        SharedPointer<DisplayWindow> _window        = nullptr;
        SharedPointer<ITextureView>  _texture       = nullptr;
        Vector2F                     _position      = {};
        Vector2F                     _direction     = {};
        Float32                      _speed         = {};
        Float32                      _rotation      = {};
        Float32                      _rotationSpeed = {};
    };

    // Derived application
    class SpriteBatching : public Application
    {
    public:
        SpriteBatching() noexcept = default;

    private:
        // Loads graphics content here!
        void LoadContent() noexcept override final
        {
            SetVSyncEnabled(false);
            SetFixedTimeStepEnabled(false);

            WString executableDirectoryPath = Path::GetExecutableDirectoryPath();
            WString texturePaths[]          = {executableDirectoryPath, L"Asset\\Sprite.png"};

            // Path to the asset file
            WString texturePath = Path::CombinePath(texturePaths);

            TextureLoadConfiguration loadConfiguration = {};
            loadConfiguration.Binding                  = TextureBinding::Sampled;
            loadConfiguration.GraphicsDevice           = GetGraphicsDevice();
            loadConfiguration.ImmediateDeviceContext   = GetImmediateGraphicsContext();
            loadConfiguration.IsSRGB                   = false;
            loadConfiguration.GenerateMip              = true;
            loadConfiguration.Usage                    = ResourceUsage::Immutable;

            TextureLoader loader = TextureLoader(FileStream(texturePath, FileMode::Binary | FileMode::Read),
                                                 loadConfiguration);

            // Creates the texture from the loaded image.
            _loadedImage = loader.CreateTexture();

            // Creates texture view for the loaded image.
            _loadedImageView = _loadedImage->CreateDefaultTextureView();

            _spriteBatch.EmplaceConstruct(GetGraphicsDevice(),
                                          GetImmediateGraphicsContext(),
                                          GetSwapChain(),
                                          SpriteBatch::MaximumMaxSpritesPerBatch);

            WString fontPaths[]  = {executableDirectoryPath.GetCString(), L"Asset\\CascadiaCodePL-SemiBold.ttf"};
            WString fontFilePath = Path::CombinePath(fontPaths);

            FontAtlasConfiguration fontAtlasConfiguration = {
                .GraphicsDevice         = GetGraphicsDevice(),
                .ImmediateDeviceContext = GetImmediateGraphicsContext(),
                .GenerateMip            = true,
                .Usage                  = ResourceUsage::Immutable,
                .Binding                = TextureBinding::Sampled,
                .DeviceQueueFamilyMask  = Math::AssignBitToPosition(Size{0}, GetImmediateGraphicsContext()->DeviceQueueFamilyIndex, true),
                .StartCharacterRange    = WChar(32),
                .EndCharacterRange      = WChar(126),
                .UseCharacterRange      = true};

            FileStream fontFile = FileStream(fontFilePath, FileMode::Binary | FileMode::Read);

            _spriteFont = Axis::System::MakeShared<SpriteFont>(fontFile,
                                                               24,
                                                               fontAtlasConfiguration);

            // Creates mouse object
            _mouse.EmplaceConstruct(GetWindow());
        }

        // Updates loop goes here!
        void Update(const TimePeriod& deltaTime) noexcept override final
        {
            // If users presses escape, the application will exit.
            if (Keyboard::GetKeyboardState().IsKeyDown(Key::Escape))
                Exit();

            _framePassed++;
            _rotation += (Float32)(deltaTime.GetTotalSeconds() * 45.0);
            _timeElapsed += deltaTime;

            MouseState mouseState = _mouse->GetMouseState();

            // Generates new sprites everytime user clicks the mouse.
            if (mouseState.GetLeftButtonState() == ButtonState::Pressed && _oldButtonState == ButtonState::Released)
            {
                Random random = {};

                for (Size i = 0; i < 2500; i++)
                {
                    Vector2F direction = {_random.Next(-100, 100), _random.Next(-100, 100)};
                    direction.Normalize();

                    _sprites.EmplaceBack(GetWindow(),
                                         _loadedImageView,
                                         Vector2F(mouseState.GetPosition().X, mouseState.GetPosition().Y), direction,
                                         160 + 80.0f * (Float32)_random.NextDouble(),
                                         (Float32)_random.Next(45, 80));
                }
            }

            _oldButtonState = mouseState.GetLeftButtonState();

            for (auto& sprite : _sprites)
                sprite.Update(deltaTime);

            _framePassed = 0;
            _timeElapsed = {};
        }

        // Render loop goes here!
        void Render(const TimePeriod& deltaTime) noexcept override final
        {
            auto currentColorView = GetSwapChain()->GetCurrentRenderTargetView();

            GetImmediateGraphicsContext()->ClearRenderTarget(currentColorView, ColorF::GetCornflowerBlue());

            _spriteBatch->Begin();

            for (auto& sprite : _sprites)
                sprite.Draw(*_spriteBatch);

            WString fpsString = L"FPS: ";
            fpsString += WString::ToString((Size)(1.0 / deltaTime.GetTotalSeconds()));

            _spriteBatch->DrawString(_spriteFont, fpsString, Vector2F(0, 0), ColorF::GetBlack());

            WString spriteCountString = L"Sprite: ";
            spriteCountString += WString::ToString(_sprites.GetLength());

            _spriteBatch->DrawString(_spriteFont, spriteCountString, Vector2F(0, _spriteFont->GetLineHeight()), ColorF::GetBlack());

            _spriteBatch->End();
        }

        // Private members
        Nullable<SpriteBatch>       _spriteBatch     = nullptr;
        Nullable<Mouse>             _mouse           = nullptr;
        SharedPointer<ITexture>     _loadedImage     = nullptr;
        SharedPointer<ITextureView> _loadedImageView = nullptr;
        SharedPointer<SpriteFont>   _spriteFont      = nullptr;
        List<Sprite>                _sprites         = {};
        Size                        _framePassed     = 0;
        TimePeriod                  _timeElapsed     = {};
        ButtonState                 _oldButtonState  = {};
        Float32                     _rotation        = 0.0f;
        Random                      _random          = {};
    };

    SpriteBatching application = {};

    application.Run();

    return 0;
}
