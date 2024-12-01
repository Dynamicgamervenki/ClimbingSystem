# Climbing System  

This repository contains a **Third-Person Wall Climbing System** developed in **Unreal Engine 5** using **C++**. It provides robust climbing mechanics for third-person games, enabling seamless interactions with walls, ledges, and other climbable objects.  

## Features  

- **Wall Climbing**:  
  The player can climb walls by interacting with climbable surfaces.  

- **Automatic Top Transition**:  
  The system smoothly transitions the player to the top of a wall upon reaching the edge.  

- **Climbing Down**:  
  Allows players to climb down ledges with smooth animations and controls.  

- **Fully C++**:  
  The entire system is implemented using Unreal Engine's C++ API for optimal performance and flexibility.  

## Getting Started  

### Prerequisites  
- Unreal Engine 5 (latest version recommended).  
- Basic understanding of Unreal Engine and C++ programming.  

### Installation  
1. Clone this repository:  
   ```bash  
   git clone https://github.com/Dynamicgamervenki/ClimbingSystem.git
   
2. **Navigate to the project directory**:  
   ```bash  
   cd ClimbingSystem  
   ```  

3. **Generate project files**:  
   - If you're using Windows, right-click the `.uproject` file and select **"Generate Visual Studio project files"**.  
   - Alternatively, you can use the command line:  
     ```bash  
     UnrealBuildTool.exe -projectfiles -project="ClimbingSystem.uproject" -game -engine  
     ```  

4. **Open the generated solution file (`.sln`)** in your preferred IDE (e.g., Visual Studio or Rider).  

5. **Build the project in Development Editor mode** to generate binaries:  
   - In Visual Studio:  
     - Set the build configuration to **Development Editor**.  
     - Select your platform (e.g., Win64).  
     - Click **Build**.  

6. **Launch Unreal Engine 5**:  
   - Double-click the `.uproject` file to open the project in the Unreal Editor.  

7. **Wait for DerivedDataCache (DDC) files to generate**:  
   - This process may take some time depending on your system and the assets in the project.  

8. **Test the climbing system**:  
   - Press **Play** in the Unreal Editor to experience the climbing system!  

