#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>

// ============================================================================
// LAB SECTION 1: STORAGE DURATION & LINKAGE DEMO
// ============================================================================
namespace StorageDemo {
    // 1. Static Storage Duration (Global Namespace Scope)
    // Adding 'static' gives this variable INTERNAL LINKAGE. If this file were
    // compiled alongside other .cpp files, they could NOT see or clash with this.
    static int globalTracker = 100;

    void demonstrateLifetimes() {
        // 2. Automatic Storage Duration (Stack Allocation)
        // Born when this block starts, dies when it ends. Resets every call.
        int automaticVar = 1;
        
        // 3. Static Storage Duration (Local Function Scope)
        // Born the first time this line hits, lives until the program exits.
        // It remembers its state between function calls!
        static int localStaticVar = 1;

        automaticVar++;
        localStaticVar++;

        std::cout << "[Lifetime Demo] Automatic Var: " << automaticVar 
                  << " | Local Static Var: " << localStaticVar << "\n";
    }
}

// ============================================================================
// LAB SECTION 2: CLASS HOLDING A STATIC INSTANCE OF ITSELF
// ============================================================================
namespace DesignPatternsDemo {
    
    // --- THE PROTOTYPE PATTERN ---
    // Useful for fast object creation by cloning a pre-configured template.
    class Particle {
    public:
        std::string texture;
        int speed;
        int lifespan;

        // A static instance of its own type! It acts as our master template.
        static Particle defaultTemplate;

        // Utility to clone the master template and return a unique instance
        static Particle createFromTemplate(std::string customTexture) {
            Particle clone = defaultTemplate; // Copies the static master
            clone.texture = customTexture;    // Customizes it slightly
            return clone;                     // Returns the unique instance
        }
    };

    // Explicit definition of the static member variable required by C++
    Particle Particle::defaultTemplate{"spark.png", 10, 60};
}


// ============================================================================
// LAB SECTION 3: INTERACTIVE EXERCISES FOR YOU TO SOLVE
// ============================================================================
namespace SandboxExercises {

    // ------------------------------------------------------------------------
    // EXERCISE 1: THE MONOSTATE PATTERN (HIVE-MIND STATE)
    // Goal: Make all instances of 'Alien' share the exact same 'damageMultiplier' 
    // variable using a static member of its own class type.
    // ------------------------------------------------------------------------
    class Alien {
    private:
        // TODO: Fix the two lines below so 'sharedConfig' is a STATIC instance 
        // of Alien, but keep 'laserPower' unique to every individual alien.
        Alien sharedConfig; 
    public:
        int laserPower = 10;
        int internalMultiplier = 1; // Used by the master config

        int calculateTotalDamage() {
            // TODO: Uncomment the line below once you make sharedConfig static!
            // return laserPower * sharedConfig.internalMultiplier;
            return laserPower; // Delete this line when solving
        }

        void setGlobalMultiplier(int val) {
            // TODO: Uncomment the line below once sharedConfig is static!
            // sharedConfig.internalMultiplier = val;
        }
    };
    // TODO: If you make sharedConfig static, remember you must define it out here!
    // Write: Alien Alien::sharedConfig;


    // ------------------------------------------------------------------------
    // EXERCISE 2: MEYERS' SINGLETON (THE GATEKEEPER)
    // Goal: Complete this Singleton so it only creates ONE AudioEngine instance,
    // using a thread-safe local static variable (lazy initialization).
    // ------------------------------------------------------------------------
    class AudioEngine {
    private:
        AudioEngine() { volume = 50; } // Private constructor avoids 'AudioEngine ae;'
    public:
        int volume;

        static AudioEngine& getInstance() {
            // TODO: Implement the Meyers' Singleton pattern here.
            // 1. Create a local static instance of AudioEngine.
            // 2. Return a reference to it.
            
            // Delete these placeholders when implementing:
            static AudioEngine placeholder; 
            return placeholder;
        }
    };


    // ------------------------------------------------------------------------
    // EXERCISE 3: DYNAMIC STORAGE DURATION (MANUAL LIFETIME)
    // Goal: This function leaks memory. Fix it by ensuring the dynamically 
    // allocated integer is safely cleaned up before the function returns.
    // ------------------------------------------------------------------------
    int memoryManagementLeak() {
        int* dynamicInt = new int(500); // Dynamic storage duration (Heap)
        *dynamicInt += 50;
        
        int result = *dynamicInt;

        // TODO: Fix the memory leak here without destroying 'result'.

        return result;
    }
}

// ============================================================================
// AUTOMATED TEST RUNNER (DO NOT MODIFY)
// ============================================================================
int main() {
    std::cout << "=== STARTING C++ CODING LAB ===\n\n";

    std::cout << "--- Testing Storage Lifetimes ---\n";
    StorageDemo::demonstrateLifetimes();
    StorageDemo::demonstrateLifetimes();
    std::cout << "Notice how Automatic Var reset to 2, but Local Static continuously climbed!\n\n";

    std::cout << "--- Running Exercise Validation ---\n";
    bool allPassed = true;

    // Test Exercise 1: Monostate
    SandboxExercises::Alien alien1;
    SandboxExercises::Alien alien2;
    alien1.setGlobalMultiplier(5);
    // If static works, changing it via alien1 should affect alien2's calculations!
    if (alien2.calculateTotalDamage() == 50) {
        std::cout << "[Exercise 1] PASS: Monostate shared configuration successfully!\n";
    } else {
        std::cout << "[Exercise 1] FAIL: Aliens are not sharing a static background state.\n";
        allPassed = false;
    }

    // Test Exercise 2: Singleton
    SandboxExercises::AudioEngine& engineA = SandboxExercises::AudioEngine::getInstance();
    SandboxExercises::AudioEngine& engineB = SandboxExercises::AudioEngine::getInstance();
    engineA.volume = 85;
    if (&engineA == &engineB && engineB.volume == 85) {
        std::cout << "[Exercise 2] PASS: Singleton successfully restricted to one instance!\n";
    } else {
        std::cout << "[Exercise 2] FAIL: engineA and engineB point to different instances.\n";
        allPassed = false;
    }

    // Test Exercise 3: Memory Leak (We trust you cleaned up the pointer!)
    if (SandboxExercises::memoryManagementLeak() == 550) {
        std::cout << "[Exercise 3] PASS: Calculation correct (Make sure you called delete!).\n";
    } else {
        std::cout << "[Exercise 3] FAIL: Calculation is broken.\n";
        allPassed = false;
    }

    std::cout << "\n================================\n";
    if (allPassed) {
        std::cout << "🎉 CONGRATULATIONS! You passed all laboratory checkpoints! 🎉\n";
    } else {
        std::cout << "❌ Keep trying! Modify the TODO sections to fix the broken tests. ❌\n";
    }
    std::cout << "================================\n";

    return 0;
}
