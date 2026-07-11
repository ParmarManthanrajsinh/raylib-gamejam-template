# Code Style Conventions

This document defines the basic naming and formatting rules used in this project. Keep the style consistent across all source files.

## Naming Conventions

### Variables
Use `snake_case` for variables.

```cpp
int screen_width = 1280;
float target_frame_time = 1.0f / 60.0f;
Vector2 player_position = { 0, 0 };
```

### Functions
Use `PascalCase` for functions.

```cpp
void UpdateDrawFrame();
Game CreateGame();
```

### Structs
Use `t_PascalCase` for structs.

```cpp
struct t_Player
{
    int health;
    Vector2 position;
};
```

## Formatting Rules

### Indentation
Use **4 spaces** for each indentation level.

- Do not use tabs for indentation.
- Keep indentation consistent in all files.

### Braces
Place opening braces on a new line. Always. No exceptions.

```cpp
void UpdateDrawFrame()
{
    if (is_running)
    {
        Update();
        Draw();
    }
}
```

This applies to:
- functions
- structs
- classes
- control blocks such as `if`, `else`, `for`, `while`, and `switch`
- lambdas

### Long Function Arguments
If a function argument list is too long, place the opening parenthesis on a new line and put each argument on its own line.

```cpp
void DrawOrthogonalWire
(
    Vector2 start_position,
    Vector2 end_position,
    Color wire_color,
    float alpha,
    int line_width
)
{
    // Code
}
```

### Lambda Formatting
For lambdas used as arguments, place the opening parenthesis on a new line. Lambda capture and parameters go on one line if short, or multiple lines if needed.

```cpp
std::sort
(
    std::execution::par,
    particles.begin(),
    particles.end(),
    [](const t_Particle& a, const t_Particle& b)
    {
        return a.lifetime < b.lifetime;
    }
);
```

```cpp
auto comparator = 
[](const t_Enemy& a, const t_Enemy& b)
{
    return a.health < b.health;
};

std::sort(enemies.begin(), enemies.end(), comparator);
```

## Modern C++ Best Practices

### String Handling
Avoid `const char*` because it lacks size information, requires manual length tracking, and provides no bounds safety. Use `std::string_view` for read-only string views with zero overhead and built-in size tracking. Pass `std::string_view` by value, not by const reference, as it's trivially copyable (typically 16 bytes) and passing by reference adds unnecessary indirection. Use `std::string` when ownership and modification are needed.

For file paths and resource names, use `std::string_view` or `std::string` instead of raw C strings.

```cpp
// Avoid - raw C strings
LoadTexture("assets/player.png");

// Avoid - unnecessary const reference
std::string GetAssetPath(const std::string_view& asset_name)
{
    return std::string("assets/") + std::string(asset_name);
}

// Use - pass by value
void LoadTexture(std::string_view path)
{
    // Load texture using path.data()
}

// Use - pass by value for views
std::string GetAssetPath(std::string_view asset_name)
{
    return std::string("assets/") + std::string(asset_name);
}
```

### Arrays
Avoid C-style arrays because they decay to pointers losing size information, have no bounds checking, and cannot be used in modern C++ templates effectively. Use `constexpr std::array` with CTAD for compile-time fixed-size arrays that preserve size information and provide bounds-safe access.

```cpp
static constexpr std::array level_names = 
{
    "TUTORIAL",
    "FOREST",
    "CAVE"
};

static constexpr std::array<const char*, 6> achievement_titles = 
{
    "TARGET REACHED!",
    "IT'S ALIVE!",
    "BOOLEAN BEAUTY!",
    "MAGIC SMOKE CONTAINED!",
    "SPAGHETTI DETANGLED!",
    "CPU IS PLEASED."
};
```

### Type Casting
Avoid C-style casts because they bypass type safety, can perform dangerous reinterpretations silently, and are difficult to search for in code. Always use `static_cast<>` for well-defined conversions, making intent explicit and allowing compile-time checking.

### Resource Management
Use RAII (Resource Acquisition Is Initialization) whenever possible. Resources like memory, file handles, and graphics contexts should be managed by objects that acquire resources in constructors and release them in destructors. This prevents leaks, ensures exception safety, and makes code self-documenting.

```cpp
class t_TextureResource
{
public:
    explicit t_TextureResource(std::string_view path)
    {
        texture = LoadTexture(path.data());
    }
    
    ~t_TextureResource()
    {
        UnloadTexture(texture);
    }
    
    // Prevent copying
    t_TextureResource(const t_TextureResource&) = delete;
    t_TextureResource& operator=(const t_TextureResource&) = delete;
    
    // Allow moving
    t_TextureResource(t_TextureResource&& other) noexcept
        : texture(other.texture)
    {
        other.texture = {};
    }
    
    Texture2D GetTexture() const { return texture; }
    
private:
    Texture2D texture;
};
```

### Lambdas
Use lambdas only when capturing context or creating local callbacks. Avoid wrapping simple function calls in lambdas as it adds unnecessary overhead and reduces readability. Prefer named functions for reusable logic.

### STL Container Usage
Use STL containers in the fastest way possible by leveraging performance-focused operations.

**Pre-allocation:** Use `reserve()` when the final size is known or can be estimated. This prevents costly reallocations and memory fragmentation during growth.

```cpp
std::vector<t_Enemy> enemies;
enemies.reserve(max_enemy_count);
```

**In-place construction:** Use `emplace_back()` instead of `push_back()` to construct elements directly in the container's memory. This avoids temporary object creation and move/copy operations.

```cpp
enemies.emplace_back(position, health_type, aggression_level);
```

**Sorting:** Use `std::sort()` for general sorting. For large datasets that benefit from parallel execution, use `std::sort()` with execution policies.

```cpp
std::sort
(
    std::execution::par,
    particles.begin(),
    particles.end(),
    [](const t_Particle& a, const t_Particle& b)
    {
        return a.lifetime < b.lifetime;
    }
);
```

**Algorithm selection:** Choose the right algorithm for the task. Use `std::find()` for linear searches on small collections, `std::binary_search()` on sorted collections, and `std::unordered_set` for membership tests.

```cpp
auto it = std::find(entities.begin(), entities.end(), target_id);

std::sort(ids.begin(), ids.end());
bool exists = std::binary_search(ids.begin(), ids.end(), search_id);

std::unordered_set<int> active_ids;
```

### Container Choice
Avoid `std::vector` when `std::array` works. `std::vector` always uses heap allocation, which introduces indirection and fragmentation. Use `std::array` for fixed-size collections to keep data on the stack with predictable memory layout.

```cpp
std::array<float, 60> frame_times = {};
std::vector<t_Bullet> active_bullets;
std::vector<t_Particle> particles;
```

### Heap vs Stack
Prefer stack allocation for performance and deterministic cleanup. Use heap allocation only when necessary.

Use stack when:
- Size is known at compile time
- Data size is small
- Lifetime is scope-bound

Use heap when:
- Size is determined at runtime
- Data is large (kilobytes or more)
- Lifetime extends beyond the creating scope

```cpp
t_Player player = {};
std::array<float, 4096> audio_buffer = {};
auto world_state = std::make_unique<t_WorldState>();
std::vector<t_Chunk> terrain_chunks;
```

## General Rules

- Keep code readable and consistent.
- Prefer clear names over short names.
- Match the existing style of the project when editing old code.
- Avoid mixing formatting styles in the same file.
- Use modern C++ features over C-style constructs.
- When in doubt, prefer clarity over cleverness.

## Summary

| Element | Style |
|---------|-------|
| Variables | `snake_case` |
| Functions | `PascalCase` |
| Structs | `t_PascalCase` |
| Indentation | 4 spaces |
| Braces | New line, always |
| Lambda parentheses | New line |
| Strings | `std::string_view` (by value) / `std::string` |
| File paths | `std::string_view` (by value) / `std::string` |
| Arrays | `constexpr std::array` |
| Casts | `static_cast<>` |
| Resource Management | RAII |
| STL Performance | reserve, emplace_back, parallel algorithms |