#include "assets.h"
#include "gates.h"
#include "hex_grid.h" // IWYU pragma: keep
#include "game.h"
#include "text_util.h"
#include "audio.h"
#include <array>
#include <cmath>
#include <cstdlib>
#include <rlgl.h>

// --- Dialogs ---
namespace Dialogs
{
    constexpr std::array diag_and =
    {
        "AND gate! The beige sedan of logic gates.",
        "You ANDed that in. I see what you did there.",
        "AND. Short for 'and-venture capital' of your circuit.",
        "Classic AND. Reliable as a brick. Equally exciting.",
        "Two inputs, one output. That's the AND deal.",
        "AND so it begins...",
    };

    constexpr std::array diag_or =
    {
        "OR gate! Finally, some decision paralysis.",
        "OR you could've chosen XOR. But OR is fine, I guess.",
        "The OR gate: for when you can't commit.",
        "OR means 'one or the other'. In your case, neither.",
        "Bold OR-dering choice there.",
        "OR... you could restart. No? We're doing OR? Fine.",
    };

    constexpr std::array diag_not =
    {
        "NOT gate! Philosophically questioning everything.",
        "NOT your average gate. (See what I did there?)",
        "Invert! Invert! The NOT gate has no retort!",
        "NOT. The 'well actually' of logic gates.",
        "0 becomes 1. 1 becomes 0. NOT is a chaotic neutral.",
        "NOT bad for a first gate.",
    };

    constexpr std::array diag_xor =
    {
        "XOR gate! Fancy! You read the manual!",
        "XOR means 'exclusive or'. You feel exclusive now, right?",
        "Ooh, XOR. Somebody's compensating for something.",
        "The XOR gate: for when you want your circuit to have *opinions*.",
        "XOR marks the spot. (Treasure not included.)",
        "XORing you been keeping those skills a secret?",
    };

    constexpr std::array diag_nand =
    {
        "NAND gate! The people's gate. Very democratic.",
        "NAND: NOT + AND = NOT what you expected? Well played.",
        "Fun fact: every circuit can be built with just NAND gates. You're welcome.",
        "NANDing around, are we?",
        "The NAND gate: passive-aggressive AND.",
        "NAND said, 'I'm not like other gates.'",
    };

    constexpr std::array diag_nor =
    {
        "NOR gate. The 'nope' of logic gates.",
        "NOR-mal Tuesday with this gate.",
        "NOR: the gate that says 'neither'. Very fence-sitter energy.",
        "You NOR-ed that one in. I'm NOR-mally impressed.",
        "The NOR gate: gate of the year 1982.",
        "NOR you going to use any other gates?",
    };

    constexpr std::array diag_xnor =
    {
        "XNOR gate! Equality matters in this household.",
        "XNOR: the 'same difference' gate.",
        "XNOR is like XOR's chill twin. Same but different. Wait—",
        "XNORing intensifies.",
        "XNOR chosen. A person of sophistication, clearly.",
        "XNOR: for people who say 'well, actually, they're the same picture.'",
    };

    constexpr std::array diag_first_gate =
    {
        "First gate! I'm so proud! (I have no idea if that's a good choice.)",
        "The journey of a thousand hexes begins with a single gate.",
        "Gate #1! Mark this day on your calendar! (Don't actually.)",
    };

    constexpr std::array diag_wire_connected =
    {
        "Zzzzt! Wired! It's ALIVE! (Not really alive.)",
        "The spaghetti grows! Mangia!",
        "One connection closer to... something.",
        "We did it. We connected two points. Nobel prize when?",
        "Wires: the unsung heroes of the digital age.",
        "That's clean. I mean, it's a straight line. But it's YOUR straight line.",
        "Routing success! *slow clap*",
        "Signal flowing! Probably. Electrical signals are shy.",
    };

    constexpr std::array diag_first_wire =
    {
        "First wire! They grow up so fast!",
        "The first connection. Cherish this moment. It's all downhill from here.",
        "A baby wire is born! *wipes tear*",
    };

    constexpr std::array diag_wire_deleted_disconnected =
    {
        "NOOOO! My beautiful wire!",
        "Cutting ties, I see. Very dramatic.",
        "Signal lost. Metaphor for something.",
        "A wire died for this moment. I hope you're happy.",
        "You disconnected it. It had dreams, you know.",
    };

    constexpr std::array diag_gate_deleted =
    {
        "MURDERER! That gate had a family!",
        "Into the void it goes. *solemn music*",
        "That gate was just doing its best. Was that not good enough for you?",
        "Delete! Delete! The gate is gone. You monster.",
        "RIP little gate. You were NOT forgotten. (Well, maybe.)",
        "'Delete' is not a strategy. Actually... it kinda is.",
        "That gate was EMPLOYED. It had DEPENDENTS.",
        "Logic gate rights violation! I'm documenting this.",
    };
    constexpr std::array diag_clear =
    {
        "NUKED! FROM ORBIT! The whole board!",
        "Reset! Fresh start! New you! (Same terrible wiring skills.)",
        "And just like that, poof. Gone. Like my motivation.",
        "Ctrl+Alt+Delete life.",
        "You hit the 'I give up' button. I respect the honesty.",
        "Clear! Clean slate! Surely this time will be different!",
    };

    constexpr std::array diag_normal_solve =
    {
        "YESSS! YOU DID IT! I NEVER DOUBTED YOU FOR A SECOND! (I doubted you a LOT.)",
        "BOOM! TARGET REACHED! HEX PUNCHER SUPREME!",
        "SOLVED! *robot breakdances poorly* WITNESS ME!",
        "EXCELLENT WORK! Your ancestors are proud. Well, the smart ones.",
        "Circuit complete! Brains: fully engaged! (Finally.)",
        "WINNER WINNER CHICKEN DINNER! The chicken is binary. But still!",
        "You did it! Target 0x%X acquired! I'm telling everyone! (There's no everyone.)",
        "Output matches target! This is not a drill! (It's a circuit.)",
        "VICTORY ROYALE! (Sorry, wrong genre.)",
        "The hex is cast! ...No wait, the hex is MATCHED!",
    };

    constexpr std::array diag_slow_solve =
    {
        "You did it! Eventually! Like a slow cooker of logic!",
        "Solved! At 0x%X! Only took you... *checks nonexistent watch*... a while.",
        "It's done. Was it worth the wait? The crowd is asleep. But it's done!",
    };
    constexpr std::array diag_fast_solve =
    {
        "UNDER 15 SECONDS?! Are you a computer?! ...Wait, you are.",
        "Speed solves! You're either a genius or very lucky. (Probably lucky.)",
        "That was... disturbingly fast. I'm concerned for your mental health.",
    };

    constexpr std::array diag_level_start =
    {
        "New puzzle! Target 0x%X. Good luck. You'll need it.",
        "A fresh circuit! The possibilities are endless! (They're actually quite limited.)",
        "Round two! Or three! Or... I've lost count. Target: 0x%X.",
        "Target: %d in hex? That's 0x%X. I'll let you work it out.",
    };

    constexpr std::array diag_hints =
    {
        "Need a hint? Check bit %d. It's supposed to be %d.",
        "Look at bit %d. It's outputting %d instead of %d.",
        "Target is 0x%X. You have %d matching bits.",
        "Are you stuck? Check your logic on bit %d.",
    };
    constexpr std::array diag_near_zone =
    {
        "I see you looking at me. I'm not a museum exhibit.",
        "You know I can see you, right?",
        "Stop staring. It's weird.",
        "If you're trying to intimidate me, I'm a robot. I don't feel fear. (Much.)",
        "You're hovering. Is this a date?",
        "I know I'm cute. Let's move on.",
        "Do you need me? Or are you just... admiring?",
        "You're blocking my view of the circuit. Go build something.",
    };
    constexpr std::array diag_close_zone =
    {
        "Personal space! I have BOUNDARIES!",
        "TOO CLOSE! Back off! I'll call the cyber-police!",
        "OK this is getting weird. I'm a 2D polygon. Let's keep it professional.",
        "EYES UP HERE, BUDDY. (Points at antenna.)",
        "You're really going for it, huh. No shame.",
        "I'm flattered but I don't date outside my resolution.",
    };
    constexpr std::array diag_boop =
    {
        "Did you just BOOP me?! I'M A MACHINE! ...Do it again.",
        "BOOP! I HAVE BEEN BOOPED! System rebooting...",
        "The sacred boop has been performed. You are now a Hex Booper.",
        "Boop detected. Filing complaint with HR.",
        "*boop* ...That's it? You came all the way over here to boop me?",
    };
    constexpr std::array diag_hesitation =
    {
        "You good? You've been staring at the screen for a while.",
        "Tick tock... that's the sound of your puzzle not solving itself.",
        "Hello? Is anyone home? The circuit is waiting.",
        "I've been watching you not move for %d seconds. Riveting stuff.",
        "Did you get disconnected? Or just disconnected from the will to live?",
        "You know, staring doesn't change the logic. (It actually does sometimes.)",
    };
    constexpr std::array diag_speed =
    {
        "WOAH THERE! Slow down! The gates can't keep up!",
        "You're moving like a caffeinated squirrel. Is this helping?",
        "HEY! My optic sensors can't track that fast!",
        "ZOOOOOM! What's the rush? The hex isn't going anywhere.",
    };
    constexpr std::array diag_obstacle =
    {
        "That's an obstacle. The red thing. With the X. Not a place for gates.",
        "You can't put a gate there. It's a restricted zone. Hex Berlin Wall.",
        "That spot is taken. By a SPIKY RED THING. Read the room.",
        "I admire your persistence. That's still an obstacle.",
        "Obstacle: 1. You: 0. Try again.",
        "The gate said 'no thanks' to that location.",
    };
    constexpr std::array diag_input_pin =
    {
        "That's an input pin. It sends signals out. It's extroverted.",
        "Click to start a wire! Or just hover menacingly. Your call.",
        "Signal source right here. 1 or 0. Deep stuff.",
    };
    constexpr std::array diag_gate_pin =
    {
        "Pins! The gate's tiny hands.",
        "That's where the magic happens. (Electric magic.)",
        "Click to connect. Or don't. I'm not your supervisor.",
    };
    constexpr std::array diag_output_pin =
    {
        "The final destination! Route your wire here for glory!",
        "Output node. Where dreams come to become hex values.",
        "Connect this and you're one step closer to fame and fortune! (Not guaranteed.)",
    };
    constexpr std::array diag_reroll =
    {
        "CHEATER DETECTED! I SAW THAT! REROLLING IS CHEATING!",
        "REROLL?! You think you can just change the target?!",
        "I can't believe you'd reroll. Actually, I totally can.",
        "DISHONOR! DISHONOR ON YOU! DISHONOR ON YOUR COW!",
        "Imagine needing to reroll the target in a puzzle game. Couldn't be you. (It's you.)",
        "REPORTED! ...There's no report button. But MENTALLY reported!",
        "The jam judges are WATCHING you right now. They SEE the R key.",
        "You're not supposed to do that. But you did. How do you sleep at night?",
        "Rolled a target you didn't like? So you REROLED. Pathetic. Love it.",
        "R-key pressed. Integrity: decreased. Shamelessness: increased.",
        "The 'R' stands for 'Really?' Because really?",
        "You must be the guy who savescum in roguelikes. I SEE you.",
    };
    constexpr std::array diag_increment_toggle =
    {
        "T-Key?! TOGGLING THE TARGET?! THAT'S CHEATING!",
        "Oh look, someone can't solve 0x%X so they just... changed it.",
        "I'm telling the devs. Oh wait, I'm IN the game. I'M WATCHING YOU.",
        "You changed the target. The puzzle is supposed to be HARD sometimes.",
        "You just T-posed on the entire game. (Because T key.)",
        "Imagine if I could change my personality with one button. ...Actually that'd be nice.",
        "T for Target. T for Tampering. T for 'That's not allowed.'",
        "The output doesn't match? Just change the target! *taps forehead* Genius!",
        "You know you're only cheating yourself, right? (And me. You're disappointing ME.)",
        "0x%X was too hard? So you cycled to 0x%s. I see how it is.",
    };
    constexpr std::array diag_first_cheat_of_session =
    {
        "OH HO! FIRST CHEAT OF THE SESSION! We have a LIVE one!",
        "And there it is. The first R key of the day. I'm not mad, I'm disappointed.",
    };
    constexpr std::array diag_zero_gate_solve =
    {
        "YOU SOLVED IT WITH ZERO GATES?! That's... statistically improbable!",
        "0 gates. Zero. Zilch. The circuit is empty. And yet... target matched.",
        "I didn't even SEE you build anything. Did I blink? Do I blink?",
        "The laziest solution possible. I'm not mad, I'm impressed.",
        "You put NOTHING on the board and it WORKED. You're a witch.",
        "The ghost in the machine solved it for you. Or you got lucky. Probably lucky.",
    };

    constexpr std::array diag_spaghetti =
    {
        "That's a lot of wires. Are you building a circuit or a bird's nest?",
        "Your wire-to-gate ratio is off the charts. It's... beautiful spaghetti.",
        "Spaghetti level: ITALIAN. Consider some routing discipline.",
        "Your circuit looks like tangled headphones. I'm getting anxious.",
        "I've seen cleaner wiring in a dumpster fire. But carry on.",
    };

    constexpr std::array diag_efficiency =
    {
        "6 gates and still no solution? You're building a monument, not a circuit.",
        "You've placed %d gates. The answer is probably simpler. But go off.",
        "Complexity level: Ancient Egyptian. You're building a pyramid of logic.",
        "More gates! MORE! Maybe the solution is in the NEXT one!",
        "Your circuit has more parts than I have lines of code. Which is saying something.",
    };

    constexpr std::array diag_progress_bits_increased =
    {
        "%d bits matching! Progress! I'm proud. (Mostly surprised.)",
        "One bit closer! The hex is in sight! (Maybe.)",
        "Getting warmer! Like a game of thermal binary Marco Polo.",
        "%d of 4 bits. That's %d%%! I did the math. You're welcome.",
    };

    constexpr std::array diag_regression_bits_decreased =
    {
        "And... you lost a bit. That's called 'regression testing' in the real world.",
        "You had %d bits! Now you have less! That's not how circuits work!",
        "Backwards! Like a crab! In a circuit!",
        "You took one step forward and two steps back. Classic.",
    };
    constexpr std::array diag_close_3_bits =
    {
        "3 BITS MATCHING! SO CLOSE! DON'T SCREW IT UP! (You'll screw it up.)",
        "One bit away! The tension is UNBEARABLE! (It's actually bearable.)",
        "3 down. 1 to go. The final frontier. Don't mess this up. You'll mess this up.",
        "You're so close I can almost taste it. Tastes like copper and binary.",
    };

    constexpr std::array diag_deletion_spree =
    {
        "You've deleted %d things. You OK? Need to talk about it?",
        "Delete. Delete. Delete. That's not fixing, that's frustration.",
        "Going through some stuff? The circuit doesn't deserve your anger!",
        "The delete button is not a therapy tool. But go off, king.",
    };

    constexpr std::array diag_wire_anxiety =
    {
        "You keep starting wires and not finishing them. Commitment issues?",
        "Wire anxiety is real. I support you.",
        "Decide where you want the wire, then click. It's not a marriage proposal.",
        "You've had like 5 phantom wires. Just COMMIT.",
    };

    constexpr std::array diag_palette_hover =
    {
        "Hmm, %s gate? Interesting choice.",
        "Thinking about %s? Bold move.",
        "%s, huh? I'd make a joke but I'm too polite.",
        "Eyes on the %s. Classic.",
        "The %s gate! A person of taste.",
        "You keep looking at %s. That says something about you.",
    };
    constexpr std::array diag_menu_greeting =
    {
        "Welcome! Ready to hex some gates?",
        "Click PLAY. You know you want to.",
        "This is the title screen. Exciting, right?",
        "I'm a robot. You're a human. Together we... solve puzzles!",
        "Staring at the title won't solve itself. (It won't.)",
        "The game is right there. Just click it.",
    };

    constexpr std::array diag_howto_intro =
    {
        "This is how you play. Pay attention!",
        "Gates + wires = hex magic. Got it?",
        "Drag, drop, connect. Simple, right?",
        "It's easier than it looks. (That's a lie.)",
        "Study hard. There will be a test. (There won't.)",
        "Read the instructions! I'll wait.",
    };
    constexpr std::array diag_howto_idle =
    {
        "Are you still reading? It's just logic gates.",
        "The Truth Table is your friend. A very strict friend.",
        "1 and 1 make 1... if you use an AND gate. Keep up.",
        "Hex is just binary wearing a trench coat.",
        "Inputs on the left, outputs on the right. Don't cross the streams.",
        "If you're stuck, just try pressing buttons. It works for me.",
        "Look at the gates down there. So much potential for spaghetti.",
        "Still confused? Join the club. The club is just you.",
    };

    constexpr std::array diag_session_end =
    {
        "Going so soon? But we were having such a good time! (We weren't.)",
        "Leaving already? Fine. Take your mediocre wiring skills with you.",
        "Session stats: %d gates placed, %d wires. An interesting profile.",
        "Bye! I'll be here judging the next player. You'll be replaced.",
    };
}

using namespace Dialogs;

template<size_t N>
static std::string GetRandomDialog(const std::array<const char*, N>& list)
{
    if (N == 0) return "";
    return list[rand() % N];
}

void Robot::Speak(const std::string& text, int priority, RobotMood mood, float duration)
{
    if (priority <= bot.dialog_priority && bot.dialog_timer > 0) return;
    if (duration <= 0.0f)
    {
        duration = text.length() * 0.03f + 1.0f;
    }
    bot.current_dialog = text;
    bot.type_timer = 0.0f;
    bot.dialog_timer = duration;
    bot.dialog_duration = duration;
    bot.dialog_elapsed = 0.0f;
    bot.dialog_priority = priority;
    if
    (
        bot.current_mood != mood &&
        (
            mood == RobotMood::HAPPY ||
            mood == RobotMood::EXCITED ||
            mood == RobotMood::ANGRY
        )
    )
    {
        for (int i=0; i<8; i++)
        {
            t_RobotParticle p;
            p.pos = bot.current_pos;
            p.pos.y -= 20;
            p.vel =
            {
                static_cast<float>((rand()%100 - 50)/20.0f),
                static_cast<float>((rand()%100 - 150)/20.0f)
            };
            p.life = 0;
            p.max_life = 1.0f + (rand()%100)/100.0f;
            p.text = (rand()%2 == 0) ? '0' : '1';
            p.col = (mood == RobotMood::ANGRY) ?
                Color{255, 50, 50, 255} :
                Color{50, 255, 100, 255};
            bot.particles.push_back(p);
        }
    }
    if (mood == RobotMood::ANGRY || mood == RobotMood::SURPRISED)
    {
        bot.glitch_timer = 0.5f;
    }
    bot.current_mood = mood;
    bot.type_cursor = 0;
}

Robot::Robot()
{
    bot.blink_timer = 3.5f + (rand() % 200) / 100.0f;
}

void Robot::UpdateAnimation(float dt, Vector2 mouse_pos)
{
    bot.anim_time += dt;
    bot.boop_timer = std::max(0.0f, bot.boop_timer - dt);
    bot.proximity_cooldown = std::max(0.0f, bot.proximity_cooldown - dt);
    bot.glitch_timer = std::max(0.0f, bot.glitch_timer - dt);

    Vector2 prev_pos = bot.current_pos;

    // Position lerp toward screen target
    if (!bot.is_dragged)
    {
        if (bot.current_screen == RobotScreen::LEVEL_COMPLETE)
        {
            bot.target_pos.x = 400.0f + sinf(bot.anim_time * 2.5f) * 250.0f;
            bot.target_pos.y = 300.0f + cosf(bot.anim_time * 1.8f) * 150.0f;

            bot.current_pos.x += (bot.target_pos.x - bot.current_pos.x) * dt * 6.0f;
            bot.current_pos.y += (bot.target_pos.y - bot.current_pos.y) * dt * 6.0f;
        }
        else
        {
            bot.current_pos.x += (bot.target_pos.x - bot.current_pos.x) * dt * 4.0f;
            bot.current_pos.y += (bot.target_pos.y - bot.current_pos.y) * dt * 4.0f;
        }
    }

    bot.trail.push_back(bot.current_pos);
    if (bot.trail.size() > 25) bot.trail.erase(bot.trail.begin());

    // Blink logic
    bot.blink_timer -= dt;
    if (bot.blink_timer <= 0)
    {
        if (bot.is_blinking)
        {
            bot.is_blinking = false;
            bot.blink_timer = 3.5f + (rand() % 200) / 100.0f; // 3.5 to 5.5
        }
        else
        {
            bot.is_blinking = true;
            bot.blink_timer = 0.1f;
        }
    }

    // Eye tracking (relative to current position)
    float dx = mouse_pos.x - bot.current_pos.x;
    float dy = mouse_pos.y - bot.current_pos.y;
    float dist = sqrtf(dx*dx + dy*dy);
    float angle = atan2f(dy, dx);

    float target_r = dist > 4.0f ? 4.0f : dist;
    float target_x = cosf(angle) * target_r;
    float target_y = sinf(angle) * target_r;

    bot.eye_offset.x += (target_x - bot.eye_offset.x) * 0.12f;
    bot.eye_offset.y += (target_y - bot.eye_offset.y) * 0.12f;

    // Typewriter logic
    if (bot.dialog_timer > 0)
    {
        bot.dialog_elapsed += dt;
        if (bot.type_cursor < static_cast<int>(bot.current_dialog.length()))
        {
            bot.type_timer += dt;
            if (bot.type_timer > 0.03f)
            {
                bot.type_timer = 0.0f;
                bot.type_cursor++;
                if (bot.type_cursor % 2 == 0) PlaySfx(SfxType::ROBOT_TALK);
            }
        }
        else
        {
            bot.dialog_timer -= dt;
        }
    }
    else
    {
        bot.dialog_priority = 0;
        bot.current_mood = RobotMood::IDLE;
    }

    // Velocity calc for squash/stretch
    bot.vel.x = (bot.current_pos.x - prev_pos.x) / dt;
    bot.vel.y = (bot.current_pos.y - prev_pos.y) / dt;

    // Spring physics for antenna
    float target_antenna = -bot.vel.x * 0.05f;
    float spring_f = (target_antenna - bot.antenna_offset) * 15.0f; // k
    bot.antenna_vel += spring_f * dt;
    bot.antenna_vel *= 0.85f; // damp
    bot.antenna_offset += bot.antenna_vel * dt;

    // Update particles
    for (int i = static_cast<int>(bot.particles.size() - 1); i >= 0; i--)
    {
        bot.particles[i].life += dt;
        bot.particles[i].pos.x += bot.particles[i].vel.x;
        bot.particles[i].pos.y += bot.particles[i].vel.y;
        if (bot.particles[i].life >= bot.particles[i].max_life)
        {
            bot.particles.erase(bot.particles.begin() + i);
        }
    }
}

void Robot::Update
(
    float game_anim_time,
    Vector2 mouse_pos,
    float level_timer[[maybe_unused]],
    int output_bits[4],
    int target_hex,
    bool solved,
    float mouse_still_time[[maybe_unused]],
    float last_action_time,
    float& idle_timer,
    [[maybe_unused]]int gate_type_counts[7],
    int delete_count[[maybe_unused]],
    int wire_count[[maybe_unused]],
    int obstacle_attempts[[maybe_unused]],
    int& matching_bits_prev
)
{
    float dt = GetFrameTime();
    UpdateAnimation(dt, mouse_pos);

    // Interaction states
    static Vector2 prev_mouse_pos = mouse_pos;
    float mouse_dx = mouse_pos.x - prev_mouse_pos.x;
    float mouse_dy = mouse_pos.y - prev_mouse_pos.y;
    float mouse_speed = sqrtf(mouse_dx*mouse_dx + mouse_dy*mouse_dy) / dt;
    prev_mouse_pos = mouse_pos;

    float dx = mouse_pos.x - bot.current_pos.x;
    float dy = mouse_pos.y - bot.current_pos.y;
    float dist = sqrtf(dx*dx + dy*dy);

    if (bot.high_five_timer > 0.0f)
    {
        bot.high_five_timer -= dt;
    }
    else if ((rand() % 2000) == 0 && bot.current_screen == RobotScreen::PLAYING)
    {
        bot.high_five_timer = 3.0f;
        bot.high_five_active = true;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if
        (
            bot.high_five_active && dist < 60.0f &&
            mouse_pos.x > bot.current_pos.x + 10.0f &&
            mouse_pos.y < bot.current_pos.y
        )
        {
            bot.high_five_active = false;
            bot.high_five_timer = 0.0f;
            bot.current_mood = RobotMood::HAPPY;
            PlaySfx(SfxType::ROBOT_BOOP); // Replace with clap sound if available
            for (int i=0; i<5; i++)
            {
                t_RobotParticle p; p.pos = bot.current_pos; p.pos.x += 40; p.pos.y -= 20;
                p.vel = {static_cast<float>(rand()%100-50), static_cast<float>(rand()%100-50)};
                p.life = 0; p.max_life = 1.0f; p.text = '*'; p.col = {255,255,0,255};
                bot.particles.push_back(p);
            }
            Speak("HIGH FIVE!", 3, RobotMood::EXCITED);
        }
        else if (dist < 40.0f)
        {
            if (dy < -10.0f)
            {
                // Poke antenna
                bot.antenna_vel -= 30.0f;
                Speak("Hey, watch the antenna!", 3, RobotMood::ANGRY);
            }
            else if (dy > -5.0f && dy < 5.0f && dx < 15.0f)
            {
                // Poke eye
                bot.is_blinking = true;
                bot.blink_timer = 0.2f;
                bot.glitch_timer = 0.5f;
                bot.current_mood = RobotMood::SURPRISED;
                Speak("Ouch! My optical sensor!", 3, RobotMood::SURPRISED);
            }
            else
            {
                bot.is_dragged = true;
            }
        }
    }

    if (bot.is_dragged)
    {
        bot.current_pos = mouse_pos;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            bot.is_dragged = false;
            if (mouse_speed > 1000.0f)
            {
                bot.current_mood = RobotMood::SAD;
                Speak("Whoa! Too fast!", 3, RobotMood::SAD);
                bot.glitch_timer = 1.0f;
            }
            else
            {
                bot.current_mood = RobotMood::ANGRY;
                Speak("Put me down!", 3, RobotMood::ANGRY);
            }
        }
    }
    else
    {
        if (dist < 40.0f && mouse_speed > 50.0f && !IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            bot.pet_count++;
            if (bot.pet_count > 30)
            {
                bot.current_mood = RobotMood::HAPPY;
                bot.pet_timer = 2.0f;
                bot.pet_count = 0;
                t_RobotParticle p; p.pos = bot.current_pos; p.pos.y -= 30;
                p.vel = {0, -20.0f};
                p.life = 0;
                p.max_life = 1.0f;
                p.text = 3;
                p.col = {255, 100, 100, 255}; // heart
                bot.particles.push_back(p);
            }
        }
        else
        {
            bot.pet_count = 0;
        }
    }

    if (bot.pet_timer > 0.0f)
    {
        bot.pet_timer -= dt;
        if (bot.pet_timer <= 0.0f) bot.current_mood = RobotMood::IDLE;
    }

    // Patience decay
    else if (dist < 150.0f)
    {
        if (bot.proximity_cooldown <= 0.0f)
        {
            bot.proximity_cooldown = 1.5f;
            Speak(GetRandomDialog(diag_near_zone), 1, RobotMood::SASSY);
        }
    }

    // Idle & Hesitation
    if (idle_timer > 10.0f)
    {
        Speak
        (
            TextFormat
            (
                GetRandomDialog(diag_hesitation).c_str(), static_cast<int>(idle_timer)
            ), 0, RobotMood::IDLE
        );
        idle_timer = 0; // Don't spam
    }

    // Hints
    float time_since_action = game_anim_time - last_action_time;
    if (time_since_action > 25.0f && !solved && game_anim_time - bot.last_hint_time > 30.0f)
    {
        bot.last_hint_time = game_anim_time;
        // Find first mismatching bit
        int wrong_bit = -1;
        for (int i=0; i<4; i++)
        {
            if (output_bits[i] != ((target_hex >> i) & 1))
            {
                wrong_bit = i;
                break;
            }
        }
        if (wrong_bit >= 0)
        {
            RobotMood hint_mood = RobotMood::IDLE;
            if (bot.trust < 30) hint_mood = RobotMood::SASSY;
            else if (bot.trust >= 70) hint_mood = RobotMood::HAPPY;
            Speak
            (
                TextFormat
                (
                    GetRandomDialog(diag_hints).c_str(), wrong_bit, ((target_hex >> wrong_bit) & 1)
                ), 2, hint_mood
            );
        }
    }

    // Patience decay
    if (time_since_action > 5.0f)
    {
        bot.patience = std::max(0.0f, bot.patience - dt * 0.1f);
    }

    // Bit matching logic
    int match_count = 0;
    for (int i=0; i<4; i++)
    {
        if (output_bits[i] == ((target_hex >> i) & 1)) match_count++;
    }
    if (match_count != matching_bits_prev)
    {
        if (match_count == 3)
        {
            Speak(GetRandomDialog(diag_close_3_bits), 3, RobotMood::EXCITED);
        }
        else if (match_count > matching_bits_prev && match_count > 0)
        {
            Speak
            (
                TextFormat
                (
                    GetRandomDialog
                    (
                        diag_progress_bits_increased
                    ).c_str(),
                    match_count,
                    match_count * 25
                ),
                2,
                RobotMood::HAPPY
            );
        }
        else if (match_count < matching_bits_prev)
        {
            Speak
            (
                TextFormat
                (
                    GetRandomDialog(diag_regression_bits_decreased).c_str(), matching_bits_prev
                ), 2, RobotMood::SAD
            );
        }
        matching_bits_prev = match_count;
    }

    // Pin hovering logic
    if (bot.is_hovering_pin)
    {
        bot.pin_hover_timer += dt;
        if (bot.pin_hover_timer > 2.0f)
        {
            if (bot.hovered_pin_type == 0)
                Speak(GetRandomDialog(diag_input_pin), 1, RobotMood::IDLE);
            else if (bot.hovered_pin_type == 1)
                Speak(GetRandomDialog(diag_gate_pin), 1, RobotMood::IDLE);
            else if (bot.hovered_pin_type == 2)
                Speak(GetRandomDialog(diag_output_pin), 1, RobotMood::IDLE);
            bot.pin_hover_timer = 0.0f; // reset so we don't spam
        }
    }
}

void Robot::Draw([[maybe_unused]]float game_anim_time, [[maybe_unused]]Vector2 mouse_pos)
{
    float pulse = 1.0f + sinf(bot.anim_time * 1.2f) * 0.025f;
    float bob_speed = 1.8f;
    float bob_amp = 2.5f;
    Color border_color = {0, 200, 255, 255};

    // Trust modulates idle bob & color
    float trust_t = bot.trust / 100.0f;
    bob_speed *= (1.8f - trust_t * 0.8f);
    bob_amp *= (1.6f - trust_t * 0.6f);
    if (bot.current_mood == RobotMood::IDLE)
    {
        border_color =
        {
            static_cast<unsigned char>(100 * (1.0f - trust_t)),
            static_cast<unsigned char>(100 + 155 * trust_t),
            255, 255
        };
    }

    // Mood visuals
    if (bot.current_mood == RobotMood::HAPPY || bot.current_mood == RobotMood::EXCITED)
    {
        bob_speed = 3.5f;
        bob_amp = 4.0f;
        border_color = {50, 255, 100, 255};
    }
    else if (bot.current_mood == RobotMood::ANGRY)
    {
        bob_speed = 5.0f;
        bob_amp = 1.5f;
        border_color = {255, 50, 50, 255};
    }
    else if (bot.current_mood == RobotMood::SAD)
    {
        bob_speed = 0.8f;
        bob_amp = 1.0f;
        border_color = {100, 100, 255, 255};
    }
    else if (bot.current_mood == RobotMood::SURPRISED)
    {
        pulse = 1.0f + sinf(bot.anim_time * 6.0f) * 0.05f;
        border_color = {255, 200, 50, 255};
    }

    float bob = sinf(bot.anim_time * bob_speed) * bob_amp;
    Vector2 p = { bot.current_pos.x, bot.current_pos.y + bob };

    // Glitch effect
    if (bot.glitch_timer > 0.0f && (rand() % 4 == 0))
    {
        p.x += (rand() % 10 - 5);
    }

    // Draw shadow removed

    // Draw Trail
    if (!bot.trail.empty())
    {
        for (size_t i = 1; i < bot.trail.size(); i++)
        {
            float alpha = static_cast<float>(i) / bot.trail.size();
            Vector2 t1 =
            {
                bot.trail[i-1].x,
                bot.trail[i-1].y +
                sinf
                (
                    (bot.anim_time - 0.05f * (bot.trail.size() - i)) * bob_speed
                ) * bob_amp
            };
            Vector2 t2 =
            {
                bot.trail[i].x,
                bot.trail[i].y +
                sinf
                (
                    (bot.anim_time - 0.05f * (bot.trail.size() - i - 1)) * bob_speed
                ) * bob_amp
            };
            DrawLineEx(t1, t2, 12.0f * alpha, ColorAlpha(border_color, alpha * 0.4f));
            DrawLineEx(t1, t2, 6.0f * alpha, ColorAlpha(WHITE, alpha * 0.8f));
        }
    }

    // Boop squish (uniform scale — DrawPoly doesn't support non-uniform)
    float boop_scale = 1.0f;
    if (bot.boop_timer > 0)
    {
        float t = bot.boop_timer / 0.3f;
        boop_scale = 1.0f - sinf(t * PI) * 0.2f;
    }

    rlPushMatrix();
    rlTranslatef(p.x, p.y, 0);

    // Squash and stretch based on velocity
    float speed = sqrtf(bot.vel.x*bot.vel.x + bot.vel.y*bot.vel.y);
    float stretch = 1.0f + speed * 0.0005f;
    float squash = 1.0f - speed * 0.00025f;
    if (stretch > 1.3f) stretch = 1.3f;
    if (squash < 0.7f) squash = 0.7f;
    rlScalef(squash, stretch, 1.0f);

    Vector2 lp = {0, 0}; // Local coordinates

    // Draw Antenna
    DrawLineEx({lp.x + bot.antenna_offset, lp.y - 42}, {lp.x, lp.y - 28}, 2.0f, border_color);
    DrawCircleV({lp.x + bot.antenna_offset, lp.y - 42}, 4.0f, border_color);

    // Draw body
    float base_radius = 32.0f * pulse * boop_scale;
    DrawPoly(lp, 6, base_radius, 90.0f, Color{16, 18, 40, 255});

    // Fake Bloom
    for(int i=1; i<=3; i++) 
    {
        DrawPolyLinesEx
        (
            lp, 
            6,
            base_radius,
            90.0f,
            2.0f + i*2.0f,
            ColorAlpha(border_color, 0.4f/i)
        );
    }
    DrawPolyLinesEx(lp, 6, base_radius, 90.0f, 2.0f, border_color);

    // Glitch chromatic aberration
    if (bot.glitch_timer > 0.0f && (rand()%3 == 0)) {
        DrawPolyLinesEx
        (
            {lp.x - 3, lp.y}, 6, base_radius, 90.0f, 2.0f, ColorAlpha(RED, 0.7f)
        );
        DrawPolyLinesEx
        (
            {lp.x + 3, lp.y}, 6, base_radius, 90.0f, 2.0f, ColorAlpha({0, 255, 255, 255}, 0.7f)
        );
    }

    // Draw Eyes
    Vector2 left_eye = { lp.x - 9, lp.y - 4 };
    Vector2 right_eye = { lp.x + 9, lp.y - 4 };

    if (bot.is_blinking)
    {
        DrawLineEx({left_eye.x - 6, left_eye.y}, {left_eye.x + 6, left_eye.y}, 2.0f, WHITE);
        DrawLineEx({right_eye.x - 6, right_eye.y}, {right_eye.x + 6, right_eye.y}, 2.0f, WHITE);
    }
    else
    {
        float eye_r = 7.0f;
        if (bot.current_mood == RobotMood::SURPRISED) eye_r = 9.0f;
        DrawCircleV(left_eye, eye_r, WHITE);
        DrawCircleV(right_eye, eye_r, WHITE);
        DrawCircleV
        (
            {left_eye.x + bot.eye_offset.x, left_eye.y + bot.eye_offset.y},
            eye_r * 0.5f, Color{20, 25, 50, 255}
        );
        DrawCircleV
        (
            {right_eye.x + bot.eye_offset.x, right_eye.y + bot.eye_offset.y},
            eye_r * 0.5f, Color{20, 25, 50, 255}
        );

        // Eyelids
        if (bot.current_mood == RobotMood::SASSY)
        {
            DrawRectangle(left_eye.x - 10, left_eye.y - 10, 20, 10, Color{16, 18, 40, 255});
            DrawRectangle(right_eye.x - 10, right_eye.y - 10, 20, 10, Color{16, 18, 40, 255});
        }
        else if (bot.current_mood == RobotMood::ANGRY)
        {
            DrawTriangle
            (
                {left_eye.x - 10, left_eye.y - 10},
                {left_eye.x - 10, left_eye.y},
                {left_eye.x + 10, left_eye.y - 10},
                Color{16, 18, 40, 255}
            );
            DrawTriangle
            (
                {right_eye.x + 10, right_eye.y - 10},
                {right_eye.x - 10, right_eye.y - 10},
                {right_eye.x + 10, right_eye.y},
                Color{16, 18, 40, 255}
            );
        }
        else if (bot.current_mood == RobotMood::SAD)
        {
            DrawTriangle
            (
                {left_eye.x + 10, left_eye.y - 10},
                {left_eye.x - 10, left_eye.y - 10},
                {left_eye.x + 10, left_eye.y},
                Color{16, 18, 40, 255}
            );
            DrawTriangle
            (
                {right_eye.x - 10, right_eye.y - 10},
                {right_eye.x - 10, right_eye.y},
                {right_eye.x + 10, right_eye.y - 10},
                Color{16, 18, 40, 255}
            );
        }
    }

    // Draw Mouth
    if (bot.current_mood == RobotMood::HAPPY || bot.current_mood == RobotMood::EXCITED)
    {
        DrawLineEx({lp.x - 5, lp.y + 10}, {lp.x, lp.y + 14}, 2.0f, WHITE);
        DrawLineEx({lp.x, lp.y + 14}, {lp.x + 5, lp.y + 10}, 2.0f, WHITE);
    }
    else if (bot.current_mood == RobotMood::SAD)
    {
        DrawLineEx({lp.x - 5, lp.y + 14}, {lp.x, lp.y + 10}, 2.0f, WHITE);
        DrawLineEx({lp.x, lp.y + 10}, {lp.x + 5, lp.y + 14}, 2.0f, WHITE);
    }
    else if (bot.current_mood == RobotMood::ANGRY || bot.patience < 20.0f)
    {
        DrawLineEx({lp.x - 6, lp.y + 12}, {lp.x - 2, lp.y + 10}, 2.0f, WHITE);
        DrawLineEx({lp.x - 2, lp.y + 10}, {lp.x + 2, lp.y + 14}, 2.0f, WHITE);
        DrawLineEx({lp.x + 2, lp.y + 14}, {lp.x + 6, lp.y + 12}, 2.0f, WHITE);
    }
    else if (bot.current_mood == RobotMood::SURPRISED)
    {
        DrawCircleV({lp.x, lp.y + 12}, 3.0f, WHITE);
    }
    else
    {
        // IDLE or SASSY
        DrawLineEx({lp.x - 4, lp.y + 12}, {lp.x + 4, lp.y + 12}, 2.0f, WHITE);
    }

    // Typing waveform
    if (bot.dialog_timer > 0 && bot.type_cursor < static_cast<int>(bot.current_dialog.length()))
    {
        float wave1 = (rand() % 6) - 3.0f;
        float wave2 = (rand() % 6) - 3.0f;
        DrawLineEx({lp.x - 3, lp.y + 12 + wave1}, {lp.x, lp.y + 12 + wave2}, 2.0f, WHITE);
        DrawLineEx({lp.x, lp.y + 12 + wave2}, {lp.x + 3, lp.y + 12 + wave1}, 2.0f, WHITE);
    }

    if (bot.high_five_timer > 0.0f)
    {
        float hand_bob = sinf(bot.anim_time * 10.0f) * 2.0f;
        DrawPoly({lp.x + 40, lp.y - 10 + hand_bob}, 5, 8.0f, 0.0f, border_color);
        DrawCircleV({lp.x + 40, lp.y - 10 + hand_bob}, 6.0f, Color{16, 18, 40, 255});
    }

    rlPopMatrix();

    // Draw Particles
    for (const auto& pt : bot.particles)
    {
        if (pt.text == 'O')
        {
            float r = (pt.life / pt.max_life) * 40.0f;
            DrawCircleLinesV(pt.pos, r, ColorAlpha(pt.col, 1.0f - (pt.life/pt.max_life)));
        }
        else
        {
            char t[2] = {pt.text, '\0'};
            DrawText
            (
                t,
                static_cast<int>(pt.pos.x),
                static_cast<int>(pt.pos.y),
                10,
                ColorAlpha(pt.col, 1.0f - (pt.life/pt.max_life))
            );
        }
    }

    // Draw Affection Meter (Trust) — only during gameplay
    if (bot.current_screen == RobotScreen::PLAYING)
    {
        float meter_w = 40.0f;
        float t = bot.trust / 100.0f;
        Color meter_color;
        if (t < 0.5f)
        {
            // Red -> Yellow
            float u = t / 0.5f;
            meter_color = {255, static_cast<unsigned char>(200 * u), 50, 255};
        }
        else
        {
            // Yellow -> Green
            float u = (t - 0.5f) / 0.5f;
            meter_color = {static_cast<unsigned char>(255 * (1.0f - u)), 255, 50, 255};
        }
        DrawRectangle
        (
            static_cast<int>(p.x - meter_w/2),
            static_cast<int>(p.y + 40),
            static_cast<int>(meter_w), 4, ColorAlpha(BLACK, 0.8f));
        DrawRectangle
        (
            static_cast<int>(p.x - meter_w/2),
            static_cast<int>(p.y + 40),
            static_cast<int>(meter_w * t), 4, meter_color
        );
    }

    // Draw Speech Bubble
    if (bot.dialog_timer > 0)
    {
        // Fade in/out
        float alpha = 1.0f;
        float elapsed = bot.dialog_elapsed;
        if (elapsed < 0.15f) alpha = elapsed / 0.15f;
        if (bot.dialog_timer < 0.3f) alpha = bot.dialog_timer / 0.3f;

        // Pop scale (bubble background only)
        float b_scale = 1.0f;
        if (elapsed < 0.1f)
        {
            b_scale = 0.8f + (elapsed / 0.1f) * 0.2f;
        }

        std::string full_text = bot.current_dialog.substr(0, bot.type_cursor);
        if (bot.type_cursor < static_cast<int>(bot.current_dialog.length())) full_text += "_";

        Font f = GetGameFont();

        // Word wrap
        std::string wrapped_text = "";
        std::string current_line = "";
        std::string current_word = "";
        float font_size = 14.0f * b_scale;

        for (char c : full_text)
        {
            if (c == ' ')
            {
                if (MeasureTextEx(f, (current_line + current_word).c_str(), font_size, 1).x > 200)
                {
                    if (!current_line.empty()) wrapped_text += current_line + "\n";
                    current_line = current_word + " ";
                }
                else
                {
                    current_line += current_word + " ";
                }
                current_word = "";
            }
            else
            {
                current_word += c;
            }
        }
        if (MeasureTextEx(f, (current_line + current_word).c_str(), font_size, 1).x > 200)
        {
            if (!current_line.empty()) wrapped_text += current_line + "\n";
            wrapped_text += current_word;
        }
        else
        {
            wrapped_text += current_line + current_word;
        }

        Vector2 size = MeasureTextEx(f, wrapped_text.c_str(), font_size, 1);
        Vector2 bubble_pos = { p.x - size.x / 2.0f, p.y - 55 - size.y };

        float pad_lr = 20.0f * b_scale;
        float pad_tb = 12.0f * b_scale;
        Rectangle bub_rect =
        {
            bubble_pos.x - pad_lr,
            bubble_pos.y - pad_tb,
            (size.x + 40) * b_scale,
            (size.y + 24) * b_scale
        };

        // Clamp to right screen edge
        if (bub_rect.x + bub_rect.width > 715)
        {
            float shift = (bub_rect.x + bub_rect.width) - 715;
            bubble_pos.x -= shift;
            bub_rect.x -= shift;
        }

        // Tail points to robot center
        float tail_w = 5.0f * b_scale;
        Vector2 tail1 = { p.x - tail_w, bub_rect.y + bub_rect.height };
        Vector2 tail2 = { p.x + tail_w, bub_rect.y + bub_rect.height };
        float tail_h = (p.y - 35) - bub_rect.y - bub_rect.height;
        Vector2 tail3 = { p.x, bub_rect.y + bub_rect.height + tail_h * b_scale };

        DrawRectangleRounded(bub_rect, 0.4f, 4, ColorAlpha(BLACK, 0.9f * alpha));
        DrawTriangle(tail3, tail1, tail2, ColorAlpha(BLACK, 0.9f * alpha));

        DrawRectangleRoundedLines
        (
            bub_rect, 0.4f, 4, ColorAlpha({0, 255, 255, 255}, 0.8f * alpha)
        );
        DrawLineEx(tail1, tail3, 2.0f, ColorAlpha({0, 255, 255, 255}, 0.8f * alpha));
        DrawLineEx(tail2, tail3, 2.0f, ColorAlpha({0, 255, 255, 255}, 0.8f * alpha));
        // Hide top line of tail
        DrawLineEx(tail1, tail2, 3.0f, ColorAlpha(BLACK, 0.9f * alpha));

        // Draw wrapped text
        float draw_y = bubble_pos.y;
        size_t pos_idx = 0;
        size_t next_pos = 0;
        while ((next_pos = wrapped_text.find('\n', pos_idx)) != std::string::npos)
        {
            std::string line = wrapped_text.substr(pos_idx, next_pos - pos_idx);
            DrawTextShadowed
            (
                f,
                line.c_str(),
                static_cast<int>(bubble_pos.x),
                static_cast<int>(draw_y),
                static_cast<int>(font_size),
                ColorAlpha(WHITE, alpha)
            );
            draw_y += font_size + 2.0f; // line spacing
            pos_idx = next_pos + 1;
        }
        std::string line = wrapped_text.substr(pos_idx);
        DrawTextShadowed
        (
            f,
            line.c_str(),
            static_cast<int>(bubble_pos.x),
            static_cast<int>(draw_y),
            static_cast<int>(font_size),
            ColorAlpha(WHITE, alpha)
        );
    }
}

void Robot::SetScreen(RobotScreen screen)
{
    if (bot.current_screen == screen) return;
    bot.current_screen = screen;
    bot.screen_dialog_timer = 0.0f;

    switch (screen)
    {
        case RobotScreen::TITLE:
            bot.target_pos = {SCREEN_WIDTH / 2.0f + 180.0f, SCREEN_HEIGHT / 2.0f - 50.0f};
            Speak(GetRandomDialog(diag_menu_greeting), 1, RobotMood::SASSY, 4.0f);
            break;
        case RobotScreen::HOW_TO_PLAY:
            bot.target_pos = {SCREEN_WIDTH - 120.0f, SCREEN_HEIGHT / 2.0f + 40.0f};
            Speak(GetRandomDialog(diag_howto_intro), 1, RobotMood::SASSY, 4.0f);
            break;
        case RobotScreen::PLAYING:
            bot.target_pos = {660.0f, 530.0f};
            break;
        case RobotScreen::LEVEL_COMPLETE:
            bot.target_pos = {680.0f, 170.0f};
            break;
    }
}

void Robot::Update(float dt, Vector2 mouse_pos)
{
    UpdateAnimation(dt, mouse_pos);

    // Proximity logic
    float dx = mouse_pos.x - bot.current_pos.x;
    float dy = mouse_pos.y - bot.current_pos.y;
    float dist = sqrtf(dx*dx + dy*dy);

    if (dist < 30.0f)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (bot.boop_timer <= 0.0f)
            {
                bot.boop_timer = 0.3f;
                PlaySfx(SfxType::ROBOT_BOOP);
                // Shockwave particle
                t_RobotParticle p;
                p.pos = bot.current_pos;
                p.vel = {0,0};
                p.life = 0;
                p.max_life = 0.5f;
                p.text = 'O'; // special ring marker
                p.col = {200,255,255,255};
                bot.particles.push_back(p);
            }
            if (bot.proximity_cooldown <= 0.0f)
            {
                bot.proximity_cooldown = 3.0f;
                Speak(GetRandomDialog(diag_boop), 3, RobotMood::SURPRISED);
            }
        }
    }
    else if (dist < 75.0f)
    {
        if (bot.proximity_cooldown <= 0.0f)
        {
            bot.proximity_cooldown = 1.5f;
            Speak(GetRandomDialog(diag_close_zone), 2, RobotMood::ANGRY);
        }
    }
    else if (dist < 150.0f)
    {
        if (bot.proximity_cooldown <= 0.0f)
        {
            bot.proximity_cooldown = 1.5f;
            Speak(GetRandomDialog(diag_near_zone), 1, RobotMood::SASSY);
        }
    }

    // Speed detection
    static Vector2 prev_mouse = mouse_pos;
    float mouse_dx = mouse_pos.x - prev_mouse.x;
    float mouse_dy = mouse_pos.y - prev_mouse.y;
    float mouse_speed = sqrtf(mouse_dx*mouse_dx + mouse_dy*mouse_dy) / dt;
    prev_mouse = mouse_pos;
    static float speed_cooldown = 0.0f;
    if (speed_cooldown > 0.0f) speed_cooldown -= dt;
    if (mouse_speed > 3000.0f && speed_cooldown <= 0.0f)
    {
        speed_cooldown = 15.0f;
        Speak(GetRandomDialog(diag_speed), 2, RobotMood::SURPRISED);
    }

    // Screen-specific periodic dialog
    bot.screen_dialog_timer += dt;
    if (bot.screen_dialog_timer > 20.0f)
    {
        bot.screen_dialog_timer = 0.0f;
        switch (bot.current_screen)
        {
            case RobotScreen::TITLE:
                Speak(GetRandomDialog(diag_menu_greeting), 1, RobotMood::SASSY);
                break;
            case RobotScreen::HOW_TO_PLAY:
                Speak(GetRandomDialog(diag_howto_idle), 1, RobotMood::SASSY);
                break;
            default: break;
        }
    }
}

// Hook Implementations
void Robot::OnGatePlaced(GateType type, int total_gates)
{
    if (total_gates == 9)
    {
        Speak
        (
            TextFormat(GetRandomDialog(diag_efficiency).c_str(), total_gates),
            2,
            RobotMood::SASSY
        );
    }
    switch(type)
    {
        case GateType::AND: Speak(GetRandomDialog(diag_and), 1); break;
        case GateType::OR: Speak(GetRandomDialog(diag_or), 1); break;
        case GateType::NOT: Speak(GetRandomDialog(diag_not), 1); break;
        case GateType::XOR: Speak(GetRandomDialog(diag_xor), 1); break;
        case GateType::NAND: Speak(GetRandomDialog(diag_nand), 1); break;
        case GateType::NOR: Speak(GetRandomDialog(diag_nor), 1); break;
        case GateType::XNOR: Speak(GetRandomDialog(diag_xnor), 1); break;
        default: break;
    }
}
void Robot::OnFirstGatePlaced([[maybe_unused]]GateType type)
{
    bot.trust = std::min(100, bot.trust + 1);
    Speak(GetRandomDialog(diag_first_gate), 2, RobotMood::HAPPY);
}
void Robot::OnWireConnected(int total_wires, [[maybe_unused]]int total_gates)
{
    if (total_wires == 11)
        Speak(GetRandomDialog(diag_spaghetti), 2, RobotMood::SASSY);
    else
        Speak(GetRandomDialog(diag_wire_connected), 1);
}
void Robot::OnFirstWireConnected()
{
    bot.trust = std::min(100, bot.trust + 1);
    Speak(GetRandomDialog(diag_first_wire), 2, RobotMood::HAPPY);
}
void Robot::OnCheat()
{
    static bool has_cheated = false;
    if (!has_cheated)
    {
        has_cheated = true;
        Speak(GetRandomDialog(diag_first_cheat_of_session), 3, RobotMood::ANGRY);
    }
}
void Robot::OnWireDragCancelled()
{
    float current_time = static_cast<float>(GetTime());
    if (current_time - bot.wire_cancel_timer < 10.0f)
    {
        bot.wire_cancel_count++;
        if (bot.wire_cancel_count >= 3)
        {
            Speak(GetRandomDialog(diag_wire_anxiety), 3, RobotMood::SAD);
            bot.wire_cancel_count = 0;
        }
    }
    else
    {
        bot.wire_cancel_count = 1;
    }
    bot.wire_cancel_timer = current_time;
}
void Robot::OnWireDeleted()
{
    float current_time = static_cast<float>(GetTime());
    if (current_time - bot.last_delete_time < 2.0f) bot.recent_deletes++;
    else bot.recent_deletes = 1;
    bot.last_delete_time = current_time;

    if (bot.recent_deletes >= 5)
    {
        Speak
        (
            TextFormat
            (
                GetRandomDialog(diag_deletion_spree).c_str(), bot.recent_deletes
            ), 3, RobotMood::SAD
        );
    }

    else Speak(GetRandomDialog(diag_wire_deleted_disconnected), 3, RobotMood::SAD);
    bot.trust = std::max(0, bot.trust - 1);
}
void Robot::OnGateDeleted([[maybe_unused]]GateType type, [[maybe_unused]]int total_gates)
{
    float current_time = static_cast<float>(GetTime());
    if (current_time - bot.last_delete_time < 2.0f) bot.recent_deletes++;
    else bot.recent_deletes = 1;
    bot.last_delete_time = current_time;

    bot.trust = std::max(0, bot.trust - 1);
    if (bot.recent_deletes >= 3)
    {
        bot.patience = std::max(0.0f, bot.patience - 5.0f);
        Speak
        (
            TextFormat(GetRandomDialog(diag_deletion_spree).c_str(), bot.recent_deletes),
            3,
            RobotMood::SAD
        );
    }
    else Speak(GetRandomDialog(diag_gate_deleted), 3, RobotMood::SAD);
}
void Robot::OnClearPressed()
{
    bot.trust = std::max(0, bot.trust - 1);
    Speak(GetRandomDialog(diag_clear), 2, RobotMood::SURPRISED);
}
void Robot::OnSolved(int total_gates, int total_wires, float level_timer)
{
    bot.trust = std::min(100, bot.trust + (total_gates <= 5 ? 2 : 1));
    bot.patience = std::min(100.0f, bot.patience + 10.0f);
    if (total_gates == 0)
        Speak(GetRandomDialog(diag_zero_gate_solve), 4, RobotMood::SURPRISED);
    else if (total_wires > 15)
        Speak(GetRandomDialog(diag_spaghetti), 4, RobotMood::SASSY);
    else if (total_gates > 10)
        Speak(TextFormat(GetRandomDialog(diag_efficiency).c_str(), total_gates), 4, RobotMood::SASSY);
    else if (level_timer > 120.0f)
        Speak(GetRandomDialog(diag_slow_solve), 4, RobotMood::HAPPY);
    else if (level_timer < 15.0f)
        Speak(GetRandomDialog(diag_fast_solve), 4, RobotMood::EXCITED);
    else
        Speak(GetRandomDialog(diag_normal_solve), 4, RobotMood::EXCITED);
}
void Robot::OnLevelStart(int target_hex)
{
    Speak(TextFormat(GetRandomDialog(diag_level_start).c_str(), target_hex), 2);
}

void Robot::OnTutorialStep(int step)
{
    if (step == 1)
    {
        Speak("Welcome to Hex Gate! Your goal is to make the OUTPUT match the TARGET hex value. See the 4 input bits on the left? You'll use logic gates to transform them.", 3, RobotMood::HAPPY);
    }
    else if (step == 2)
    {
        Speak("First, pick a gate from the palette at the bottom. Try the OR gate.", 3, RobotMood::IDLE);
    }
    else if (step == 3)
    {
        Speak("Now place it on the grid. Click an empty hex cell to put the gate down.", 3, RobotMood::IDLE);
    }
    else if (step == 4)
    {
        Speak("Great! Click a circle (pin) on an INPUT node, then click the OR gate's input pin on the left side.", 3, RobotMood::HAPPY);
    }
    else if (step == 5)
    {
        Speak("Now connect the gate's OUTPUT pin to the OUTPUT node so the signal reaches the target checker.", 3, RobotMood::IDLE);
    }
    else if (step == 6)
    {
        Speak("You've mastered the basics! Gates placed, wires connected. Time for the real deal!", 3, RobotMood::EXCITED);
    }
}
void Robot::OnTutorialError(const std::string& error_type)
{
    if (error_type == "wrong_start_step4")
        Speak("Start from the INPUT side — click one of the small glowing circles on the left!", 3, RobotMood::IDLE);
    else if (error_type == "wrong_target_step4")
        Speak("That's not the gate's input pin. Click the small circle on the LEFT side of your OR gate.", 3, RobotMood::IDLE);
    else if (error_type == "wrong_start_step5")
        Speak("Start from your gate's OUTPUT pin — the small circle on its right side.", 3, RobotMood::IDLE);
    else if (error_type == "wrong_target_step5")
        Speak("That's not the right target. Click one of the glowing circles on the OUTPUT node (right side).", 3, RobotMood::IDLE);
}
void Robot::OnPaletteHover(GateType type)
{
    float current_time = static_cast<float>(GetTime())  ;
    int t = static_cast<int>(type);
    if (bot.last_palette_hover_type != t || current_time - bot.last_palette_hover_time > 3.0f)
    {
        bot.last_palette_hover_type = t;
        bot.last_palette_hover_time = current_time;
        std::string gate_name = GateTypeToString(type);
        std::string raw = GetRandomDialog(diag_palette_hover);
        size_t pos = raw.find("%s");
        if (pos != std::string::npos) 
        {
            raw.replace(pos, 2, gate_name);
        }
        Speak(raw, 1, RobotMood::IDLE);
    }
}
void Robot::OnObstacleAttempt()
{
    bot.trust = std::max(0, bot.trust - 1);
    Speak(GetRandomDialog(diag_obstacle), 2, RobotMood::ANGRY);
}
void Robot::SetHoveredPin(const void* pin)
{
    if (pin != nullptr)
    {
        const t_PinHit* p = static_cast<const t_PinHit*>(pin);
        if (!bot.is_hovering_pin || bot.hovered_pin_type != p->source_type)
        {
            bot.is_hovering_pin = true;
            bot.pin_hover_timer = 0.0f;
            bot.hovered_pin_type = p->source_type;
        }
    }
    else
    {
        bot.is_hovering_pin = false;
        bot.pin_hover_timer = 0.0f;
    }
}
void Robot::OnCheatDetected(const std::string& cheat_type)
{
    bot.trust = std::max(0, bot.trust - 2);
    if (cheat_type == "reroll")
        Speak(GetRandomDialog(diag_reroll), 5, RobotMood::ANGRY);
    else
        Speak(GetRandomDialog(diag_increment_toggle), 5, RobotMood::ANGRY);
}
void Robot::OnSessionEnd(int total_gates, int total_wires)
{
    Speak
    (
        TextFormat
        (
            GetRandomDialog(diag_session_end).c_str(), total_gates, total_wires
        ), 5, RobotMood::SASSY
    );
}
