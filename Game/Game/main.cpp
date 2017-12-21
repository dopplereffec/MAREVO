
#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"//подключили код с видом камеры
 
using namespace sf;
 
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player { 
private: float x, y;
public:
	float w, h, dx, dy, speed;
	int dir ;
	String File;
	Image image;
	Texture texture;
	Sprite sprite;
	Player(String F, float X, float Y, float W, float H){ 
                dx=0;dy=0;speed=0;
 
		File = F;
		w = W; h = H;
		image.loadFromFile("images/" + File);
		image.createMaskFromColor(Color(41, 33, 59));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		x = X; y = Y;
		sprite.setTextureRect(IntRect(0, 0, w, h)); 
	}
	void update(float time) 
	{
		switch (dir)
		{
		case 0: dx = speed; dy = 0; break;
		case 1: dx = -speed; dy = 0; break;
		case 2: dx = 0; dy = speed; break;
		case 3: dx = 0; dy = -speed; break;
		}
 
		x += dx*time;
		y += dy*time;
 
		speed = 0;
		sprite.setPosition(x, y); 
	}
 
	float getplayercoordinateX(){	//этим методом будем забирать координату Х	
		return x;
	}
	float getplayercoordinateY(){	//этим методом будем забирать координату Y 	
		return y;
	}
	
};
 
int main()
{
	RenderWindow window(sf::VideoMode(640, 480), "v.0.4");
	view.reset(sf::FloatRect(0, 0, 640, 480));//размер "вида" камеры при создании объекта вида камеры. (потом можем менять как хотим) Что то типа инициализации.
 
 
	Image map_image;
	map_image.loadFromFile("images/map.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);
 
	
	Player p("hero3.png", 250, 250, 82.0, 82.0);
	
 
	float CurrentFrame = 0;
	Clock clock;
 
	while (window.isOpen())
	{
 
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart(); 
		time = time / 800; 
 
 
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
	
		///////////////////////////////////////////Управление персонажем с анимацией////////////////////////////////////////////////////////////////////////
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			p.dir = 1; p.speed = 0.1;
			CurrentFrame += 0.005*time;
			if (CurrentFrame > 3) CurrentFrame -= 3;
			p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 96, 82, 82)); 
			getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());//передаем координаты игрока в функцию управления камерой
		}
 
		if (Keyboard::isKeyPressed(Keyboard::Right)) {
			p.dir = 0; p.speed = 0.1;
			CurrentFrame += 0.005*time;
			if (CurrentFrame > 3) CurrentFrame -= 3;
			p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 192, 80, 80)); 
			getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());//передаем координаты игрока в функцию управления камерой

		}
 
		p.update(time);
		window.setView(view);//"оживляем" камеру в окне sfml
		window.clear();
 
 
		/////////////////////////////Рисуем карту/////////////////////
		for (int i = 0; i < HEIGHT_MAP; i++)
		for (int j = 0; j < WIDTH_MAP; j++)
		{
			if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32)); 
			if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
			if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
 
 
			s_map.setPosition(j * 32, i * 32);
 
			window.draw(s_map);
		}
		window.draw(p.sprite);
		window.display();
	}
 
	return 0;
}