#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1920;
int screen_y = 1200;

//==================================================== DISPLAYING LEVEL =======================================================================//

void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, Texture& GhostTexture, Sprite* GhostArr, Texture& SkelTexture, Sprite* SkelArr, const int height, const int width, const int cell_size)
{
	window.draw(bgSprite);

	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{

			if (lvl[i][j] == '#')
			{
				blockSprite.setPosition(j * cell_size, i * cell_size);
				window.draw(blockSprite);
			}
		}
	}

}

//============================================================================================================================================//

//================================================== PLAYER'S VERTICAL MOTION MECHANICS ======================================================//

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_y = player_y;

	offset_y += velocityY;
	
	if (velocityY > 0) {

		char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
		char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
		char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

		if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
		{
			onGround = true;
		}
		else
		{
			player_y = offset_y;
			onGround = false;
		}
	}
	
	else {
		player_y = offset_y;
		onGround = false;
	} 

	if (!onGround)
	{
		velocityY += gravity;
		if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
	}

	else
	{
		velocityY = 0;
	}
}

//============================================================================================================================================//

//==================================================== COLLISION WITH LEFT WALL ==============================================================//

void left_collision(char** lvl, float& offset_x, bool& left_collide, float& player_x, float& player_y, float& speed, const int cell_size, int& Pheight)
{
	offset_x = player_x;

	offset_x -= speed;

	char bottom_left = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x) / cell_size];
	char top_left = lvl[(int)(player_y) / cell_size][(int)(offset_x) / cell_size];
	char left_mid = lvl[(int)(player_y + (Pheight/2)) / cell_size][(int)(offset_x) / cell_size];

	if (bottom_left == '#' || left_mid == '#' || top_left == '#')
	{
		left_collide = true;
	}
	else
	{
		player_x = offset_x;
		left_collide = false;
	}
}

//=============================================================================================================================================//

//================================================== COLLISION WITH RIGHT WALL ================================================================//

void right_collision(char** lvl, float& offset_x, bool& right_collide, float& player_x, float& player_y, float& speed, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_x = player_x;

	offset_x += speed;

	char bottom_right = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x + Pwidth) / cell_size];
	char top_right = lvl[(int)(player_y) / cell_size][(int)(offset_x + Pwidth) / cell_size];
	char right_mid = lvl[(int)(player_y + (Pheight/2)) / cell_size][(int)(offset_x + Pwidth) / cell_size];

	if (bottom_right == '#' || right_mid == '#' || top_right == '#')
	{
		right_collide = true;
	}
	else
	{
		player_x = offset_x;
		right_collide = false;
	}
}

//==============================================================================================================================================//

//===================================================== MOVING DOWN ONE PLATFORM ===============================================================//

void down_movement(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_y = player_y;
	
	offset_y += velocityY;
	
	onGround = true;
	
	char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x) / cell_size];
	char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
	char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];
	
	if (bottom_left_down == '#' && lvl[(int)(offset_y + Pheight + cell_size) / cell_size][(int)(player_x ) / cell_size] != '#' || bottom_mid_down == '#' && lvl[(int)(offset_y  + Pheight + cell_size) / cell_size][(int)(player_x + Pwidth) / cell_size] != '#' || bottom_right_down == '#' && lvl[(int)(offset_y + Pheight + cell_size) / cell_size][(int)(player_x + Pwidth / 2) / cell_size] != '#')
	{
		player_y = offset_y;
		onGround = false;
	}
	else
	{
		onGround = true;
	}

	if (!onGround)
	{
		velocityY += gravity;
		if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
	}
	else
	{
		velocityY = 0;
	}
}

//==============================================================================================================================================//

//================================================= GHOST MOVEMENT =============================================================================//

void Ghost_Movement(char** lvl, int& GhostWidth, int& GhostHeight, int& Ghost_X, int& Ghost_Y, int& Inv_Ghost_X, int& Inv_Ghost_Y, bool* GhostCollide, const int cell_size, double** GhostPositions, Sprite* GhostArr, Texture& GhostTexture, Texture& Inv_GhostTexture)
{
	float speed = 1;
	
	for (int i = 0; i < 8; i++) {
	
		float offset_x = GhostPositions[i][0];
		
		offset_x += speed;
		
		char bottom_left = lvl[(int)(GhostPositions[i][1] + GhostHeight + cell_size) / cell_size][(int)(offset_x) / cell_size];
		char left_mid = lvl[(int)(GhostPositions[i][1] + (GhostHeight/2)) / cell_size][(int)(offset_x) / cell_size];
		char bottom_right = lvl[(int)(GhostPositions[i][1] + GhostHeight + cell_size) / cell_size][(int)(offset_x + GhostWidth) / cell_size];
		char right_mid = lvl[(int)(GhostPositions[i][1] + (GhostHeight/2)) / cell_size][(int)(offset_x + GhostWidth) / cell_size];
		
		if (bottom_right != '#' || right_mid == '#') {
			GhostCollide[i] = true;
			GhostArr[i].setTexture(GhostTexture);
			GhostArr[i].setTextureRect(IntRect(Ghost_X, Ghost_Y, 36, 31));
		}
		else if (bottom_left != '#' || left_mid == '#') {
			GhostCollide[i] = false;
			GhostArr[i].setTexture(Inv_GhostTexture);
			GhostArr[i].setTextureRect(IntRect(Inv_Ghost_X, Inv_Ghost_Y, 36, 31));
		}
		if (GhostCollide[i]) {
			GhostPositions[i][0] -= speed;
		}
		else {
			GhostPositions[i][0] += speed;
		}
	}
}

//==============================================================================================================================================//

//================================================= SKELETON MOVEMENT ==========================================================================//

void Skel_Movement(char** lvl, int& SkelWidth, int& SkelHeight, int& Skel_X, int& Skel_Y, int& Inv_Skel_X, int& Inv_Skel_Y, bool* SkelCollide, const int cell_size, double** SkelPositions, Sprite* SkelArr, Texture& SkelTexture, Texture& Inv_SkelTexture)
{
	float speed = 2;
	
	for (int i = 0; i < 4; i++) {
	
		float offset_x = SkelPositions[i][0];
		
		offset_x += speed;
		
		char bottom_left = lvl[(int)(SkelPositions[i][1] + SkelHeight + cell_size) / cell_size][(int)(offset_x) / cell_size];
		char left_mid = lvl[(int)(SkelPositions[i][1] + (SkelHeight/2)) / cell_size][(int)(offset_x) / cell_size];
		char bottom_right = lvl[(int)(SkelPositions[i][1] + SkelHeight + cell_size) / cell_size][(int)(offset_x + SkelWidth) / cell_size];
		char right_mid = lvl[(int)(SkelPositions[i][1] + (SkelHeight/2)) / cell_size][(int)(offset_x + SkelWidth) / cell_size];
		
		if (bottom_right != '#' || right_mid == '#') {
			SkelCollide[i] = true;
			SkelArr[i].setTexture(SkelTexture);
			SkelArr[i].setTextureRect(IntRect(Skel_X, Skel_Y, 34, 39));
		}
		else if (bottom_left != '#' || left_mid == '#') {
			SkelCollide[i] = false;
			SkelArr[i].setTexture(Inv_SkelTexture);
			SkelArr[i].setTextureRect(IntRect(Inv_Skel_X, Inv_Skel_Y, 34, 39));
		}
		if (SkelCollide[i]) {
			SkelPositions[i][0] -= speed;
		}
		else {
			SkelPositions[i][0] += speed;
		}
	}
}

//==============================================================================================================================================//

//============================================================ GHOST SUCTION ===================================================================//

void Ghost_Suction(int& vacuum_direction, bool& vacuum_on, float& vacuum_range, int& captured_count, double** GhostPositions, float& player_x, float& player_y, Sprite* GhostArr)
{
	if (vacuum_on && captured_count < 3)
	{
		for (int i = 0; i < 8; i++)
		{
		   if (GhostPositions[i][0] < 0) 
		   	continue; // it means it is already captured

		   float dx = GhostPositions[i][0] - player_x;
		   float dy = GhostPositions[i][1] - player_y;
		   float dist = sqrt(dx * dx + dy * dy);
		   
		   if (dx < 0)
		   	dx = -dx;
		   	
		   if (dy < 0)
		   	dy = -dy;

		   if (dist < vacuum_range)
		   {
			  bool can_suck = false;

			  if (vacuum_direction == 0 && dx > 0 && dx > dy) 
			  	can_suck = true; // right
			  if (vacuum_direction == 1 && dx < 0 && dx > dy)
			  	can_suck = true; // left
			  if (vacuum_direction == 2 && dy < 0 && dy > dx) 
			  	can_suck = true; // up
			  if (vacuum_direction == 3 && dy > 0 && dy > dx) 
			  	can_suck = true; // down

			  if (can_suck)
			  {
				 if (dist > 50)
				 {
				     // Pull ghost closer
				     GhostPositions[i][0] -= (dx / dist) * 3;
				     GhostPositions[i][1] -= (dy / dist) * 3;
				 }
				 else
				 {
				     // Captured!
				     GhostPositions[i][0] = -100;
				     GhostPositions[i][1] = -100;
				     
				     captured_count++;
				 }
			  }
		   }
		}
	}
}

//==============================================================================================================================================//

//========================================================= SKELETON SUCTION ===================================================================//

void Skel_Suction(int& vacuum_direction, bool& vacuum_on, float& vacuum_range, int& captured_count, double** SkelPositions, float& player_x, float& player_y, Sprite* SkelArr)
{
	if (vacuum_on && captured_count < 3)
	{
		for (int i = 0; i < 4; i++)
		{
		   if (SkelPositions[i][0] < 0) 
		   	continue; // it means it is already captured

		   float dx = SkelPositions[i][0] - player_x;
		   float dy = SkelPositions[i][1] - player_y;
		   float dist = sqrt(dx * dx + dy * dy);
		   
		   if (dx < 0)
		   	dx = -dx;
		   	
		   if (dy < 0)
		   	dy = -dy;

		   if (dist < vacuum_range)
		   {
			  bool can_suck = false;

			  if (vacuum_direction == 0 && dx > 0 && dx > dy) 
			  	can_suck = true; // right
			  if (vacuum_direction == 1 && dx < 0 && dx > dy)
			  	can_suck = true; // left
			  if (vacuum_direction == 2 && dy < 0 && dy > dx) 
			  	can_suck = true; // up
			  if (vacuum_direction == 3 && dy > 0 && dy > dx) 
			  	can_suck = true; // down

			  if (can_suck)
			  {
				 if (dist > 50)
				 {
				     // Pull ghost closer
				     SkelPositions[i][0] -= (dx / dist) * 3;
				     SkelPositions[i][1] -= (dy / dist) * 3;
				 }
				 else
				 {
				     // Captured!
				     SkelPositions[i][0] = -100;
				     SkelPositions[i][1] = -100;
				     
				     captured_count++;
				 }
			  }
		   }
		}
	}
}

//==============================================================================================================================================//

//============================================================ MAIN FUNCTION ===================================================================//
	
int main()
{

	RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	//level specifics
	const int cell_size = 64;
	const int height = 15;
	const int width = 24;
	char** lvl;

	//level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	bgTex.loadFromFile("Asset/Sprites/bg.png");
	bgSprite.setScale(1,1);
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);

	blockTexture.loadFromFile("Asset/Sprites/block1.png");
	blockSprite.setTexture(blockTexture);

	//Music initialisation
	Music lvlMusic;

	lvlMusic.openFromFile("Asset/Sound/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);

	//player data
	float player_x = 150;
	float player_y = 700;

	float speed = 5;

	const float jumpStrength = -20; // Initial jump velocity
	const float gravity = 1.0f;  // Gravity acceleration

	bool isJumping = false;  // Track if jumping

	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;

	Texture PlayerTexture;
	Sprite PlayerSprite;

	bool onGround = false;

	float offset_x = 50;
	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 20;

	int PlayerHeight = 102;
	int PlayerWidth = 96;

	bool up_button = false;
	bool facingLeft = false;

	char top_left = '\0';
	char top_right = '\0';
	char top_mid = '\0';

	char left_mid = '\0';
	char right_mid = '\0';

	char bottom_left = '\0';
	char bottom_right = '\0';
	char bottom_mid = '\0';

	char bottom_left_down = '\0';
	char bottom_right_down = '\0';
	char bottom_mid_down = '\0';

	char top_right_up = '\0';
	char top_mid_up = '\0';
	char top_left_up = '\0';

	PlayerTexture.loadFromFile("Asset/Sprites/player.png");
	PlayerSprite.setTexture(PlayerTexture);
	PlayerSprite.setScale(3,3);
	PlayerSprite.setPosition(player_x, player_y);

//========================================================= LOADING GHOSTS =====================================================================//

	int Ghost_X = 7;
	int Ghost_Y = 8;
	int Inv_Ghost_X = 1168;
	int Inv_Ghost_Y = 8;
	int GhostWidth = 108;
	int GhostHeight = 93;
	bool GhostCollide[8];
	
	double** GhostPositions;
	GhostPositions = new double* [8];
	for (int i = 0; i < 8; i += 1)
	{
		GhostPositions[i] = new double[2];
	}
	
	for (int i = 0; i < 8; i++)
		GhostCollide[i] = false;
	
	Texture GhostTexture;
	Texture Inv_GhostTexture;
	Sprite GhostSprite;
	Sprite GhostArr[8];
	GhostTexture.loadFromFile("Asset/Sprites/Ghost.png");
	Inv_GhostTexture.loadFromFile("Asset/Sprites/Ghost_inverted.png");
	
	for (int i = 0; i < 8; i++)
		GhostArr[i] = GhostSprite;
	
	GhostPositions[0][0] = 1*cell_size;
	GhostPositions[0][1] = 5.5*cell_size;
	GhostPositions[1][0] = 1*cell_size;
	GhostPositions[1][1] = 8.5*cell_size;
	GhostPositions[2][0] = 8*cell_size;
	GhostPositions[2][1] = 2.5*cell_size;
	GhostPositions[3][0] = 16*cell_size;
	GhostPositions[3][1] = 2.5*cell_size;
	GhostPositions[4][0] = 20*cell_size;
	GhostPositions[4][1] = 8.5*cell_size;
	GhostPositions[5][0] = 11*cell_size;
	GhostPositions[5][1] = 8.5*cell_size;
	GhostPositions[6][0] = 12*cell_size;
	GhostPositions[6][1] = 5.5*cell_size;
	GhostPositions[7][0] = 18*cell_size;
	GhostPositions[7][1] = 11.5*cell_size;
	
	for (int i = 0; i < 8; i++) {
		GhostArr[i].setTexture(GhostTexture);
		GhostArr[i].setScale(3,3);
		GhostArr[i].setPosition(GhostPositions[i][0], GhostPositions[i][1]);
		GhostArr[i].setTextureRect(IntRect(Ghost_X, Ghost_Y, 36, 31));
	}
	
//=============================================================================================================================================//

//========================================================= LOADING SKELETONS =================================================================//

	int Skel_X = 6;
	int Skel_Y = 33;
	int Inv_Skel_X = 1315;
	int Inv_Skel_Y = 33;
	int SkelWidth = 102;
	int SkelHeight = 117;
	bool SkelCollide[4];
	
	double** SkelPositions;
	SkelPositions = new double* [4];
	for (int i = 0; i < 4; i += 1)
	{
		SkelPositions[i] = new double[2];
	}
	
	for (int i = 0; i < 4; i++)
		SkelCollide[i] = false;
	
	Texture SkelTexture;
	Texture Inv_SkelTexture;
	Sprite SkelSprite;
	Sprite SkelArr[4];
	SkelTexture.loadFromFile("Asset/Sprites/Skeleton.png");
	Inv_SkelTexture.loadFromFile("Asset/Sprites/Skeleton_inverted.png");
	
	for (int i = 0; i < 4; i++)
		SkelArr[i] = SkelSprite;
	
	SkelPositions[0][0] = 20*cell_size;
	SkelPositions[0][1] = 5.15*cell_size;
	SkelPositions[1][0] = 1*cell_size;
	SkelPositions[1][1] = 8.15*cell_size;
	SkelPositions[2][0] = 8*cell_size;
	SkelPositions[2][1] = 2.15*cell_size;
	SkelPositions[3][0] = 9*cell_size;
	SkelPositions[3][1] = 11.15*cell_size;
	
	for (int i = 0; i < 4; i++) {
		SkelArr[i].setTexture(SkelTexture);
		SkelArr[i].setScale(3,3);
		SkelArr[i].setPosition(SkelPositions[i][0], SkelPositions[i][1]);
		SkelArr[i].setTextureRect(IntRect(Skel_X, Skel_Y, 34, 39));
	}
	
//=============================================================================================================================================//

//========================================================= LOADING VACUUM ====================================================================//

	Texture VacuumTexture;
	Sprite VacuumSprite;

	VacuumTexture.loadFromFile("Asset/Sprites/Tumblepoppers.png");
	VacuumSprite.setTexture(VacuumTexture);
	VacuumSprite.setTextureRect(IntRect(348,174,49,29));
	VacuumSprite.setScale(3,3);

	int vacuum_direction = 0;
	bool vacuum_on = false;
	float vacuum_range = 200.0f;
	int captured_count = 0;

//=============================================================================================================================================//

	//creating level array
	lvl = new char* [height];
	for (int i = 0; i < height; i += 1)
	{
		lvl[i] = new char[width];
	}

	//Designing the Level
	for (int i = 0; i < width; i++)
		lvl[13][i] = '#';
	for (int i = 0; i < height; i++)
	{
		lvl[i][0] = '#';
		lvl[i][23] = '#';
	}
	for (int i = 1; i < width - 1; i++)
	{
		if (i > 5 && i < 9 || i > 14 && i < 18)
			continue;
		lvl[10][i] = '#';
	}
	for (int i = 1; i < width - 1; i++)
	{
		if (i > 4 && i < 8 || i > 15 && i < 19)
			continue;
		lvl[7][i] = '#';
	}
	for (int i = 6; i > 4; i--)
	{
		for (int j = 1; j < width - 1; j++)
		{
			if (j == 8 || j == 15)
				lvl[i][j] = '#';
		}
	}
	for (int i = 1; i < width - 1; i++)
	{
		if (i > 2 && i < 11 || i > 12 && i < 21)
			lvl[4][i] = '#';
	}
		
	Event ev;

	//main loop
	while (window.isOpen())
	{
		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed) 
			{
				window.close();
			}

			if (ev.type == Event::KeyPressed)
			{
			}

	}
		
	if (Keyboard::isKeyPressed(Keyboard::Left)) // Move Left
	{
		facingLeft = true;
		PlayerTexture.loadFromFile("Asset/Sprites/player.png");
		PlayerSprite.setTexture(PlayerTexture);
		left_collision(lvl, offset_x, left_collide, player_x, player_y, speed, cell_size, PlayerHeight);
	}

	if (Keyboard::isKeyPressed(Keyboard::Right)) // Move Right
	{
		facingLeft = false;
		PlayerTexture.loadFromFile("Asset/Sprites/player_inverted.png");
		PlayerSprite.setTexture(PlayerTexture);
		right_collision(lvl, offset_x, right_collide, player_x, player_y, speed, cell_size, PlayerHeight, PlayerWidth);
	}
	
	if (Keyboard::isKeyPressed(Keyboard::Up)) // Jump Up
	{
		if (onGround)
			velocityY = jumpStrength;
	}
	
	if (Keyboard::isKeyPressed(Keyboard::Down)) // Move Down
	{		down_movement(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
	}
	
	if (Keyboard::isKeyPressed(Keyboard::W)) 
	{
		vacuum_direction = 2;
	}
	if (Keyboard::isKeyPressed(Keyboard::A)) 
	{
		vacuum_direction = 1;
	}
	if (Keyboard::isKeyPressed(Keyboard::S)) 
	{
		vacuum_direction = 3;
	}
	if (Keyboard::isKeyPressed(Keyboard::D)) 
	{
		vacuum_direction = 0;
	}

	if (Keyboard::isKeyPressed(Keyboard::Space)) 
	{
		vacuum_on = true;
		if (facingLeft) {
			VacuumTexture.loadFromFile("Asset/Sprites/Tumblepoppers.png");
			VacuumSprite.setTexture(VacuumTexture);
			VacuumSprite.setTextureRect(IntRect(348,174,49,29));
			VacuumSprite.setPosition(player_x - 1.55*PlayerWidth, player_y + 17);
		}
		else if (!facingLeft) {
			VacuumTexture.loadFromFile("Asset/Sprites/Tumblepoppers_inverted.png");
			VacuumSprite.setTexture(VacuumTexture);
			VacuumSprite.setTextureRect(IntRect(754,174,49,29));
			VacuumSprite.setPosition(player_x + PlayerWidth, player_y + 17);
		}
	}
	else 
	{
		vacuum_on = false;
		VacuumSprite.setPosition(-1000,-1000);
	}	
	
 	//presing escape to close
	if (Keyboard::isKeyPressed(Keyboard::Escape))
	{
		window.close();
	}
           
	window.clear();

	display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, GhostTexture, GhostArr, SkelTexture, SkelArr, height, width, cell_size);
	player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
	PlayerSprite.setPosition(player_x, player_y);
	Ghost_Movement(lvl, GhostWidth, GhostHeight, Ghost_X, Ghost_Y, Inv_Ghost_X, Inv_Ghost_Y, GhostCollide, cell_size, GhostPositions, GhostArr, GhostTexture, Inv_GhostTexture);
	Skel_Movement(lvl, SkelWidth, SkelHeight, Skel_X, Skel_Y, Inv_Skel_X, Inv_Skel_Y, SkelCollide, cell_size, SkelPositions, SkelArr, SkelTexture, Inv_SkelTexture);
	Ghost_Suction(vacuum_direction, vacuum_on, vacuum_range, captured_count, GhostPositions, player_x, player_y, GhostArr);
	Skel_Suction(vacuum_direction, vacuum_on, vacuum_range, captured_count, SkelPositions, player_x, player_y, SkelArr);
	window.draw(PlayerSprite);
	for (int i = 0; i < 8; i++) {
		GhostArr[i].setPosition(GhostPositions[i][0], GhostPositions[i][1]);
		window.draw(GhostArr[i]);
	}
	for (int i = 0; i < 4; i++) {
		SkelArr[i].setPosition(SkelPositions[i][0], SkelPositions[i][1]);
		window.draw(SkelArr[i]);
	}
	window.draw(VacuumSprite);
	window.display();
			
	}

	//stopping music and deleting level array
	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;

	return 0;
}

//===============================================================================================================================================//



