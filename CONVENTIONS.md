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
Place opening braces on a new line.

```cpp
void UpdateDrawFrame()
{
    // Code
}
```

This applies to:
- functions
- structs
- classes
- control blocks such as `if`, `else`, `for`, `while`, and `switch`

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

Use this style for arguments of any type, such as `int`, `float`, `char`, `string`, `short`, `byte`, structs, classes, pointers, and references. Always include clear variable names.

### Example

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

## General Rules

- Keep code readable and consistent.
- Prefer clear names over short names.
- Match the existing style of the project when editing old code.
- Avoid mixing formatting styles in the same file.

## Summary

- Variables: `snake_case`
- Functions: `PascalCase`
- Structs: `t_PascalCase`
- Indentation: `4 spaces`
- Braces: opening brace on a new line
- Long function arguments: opening parenthesis on a new line, one argument per line
