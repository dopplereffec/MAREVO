#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
#include <SFML/Audio.hpp>
using namespace sf;
/////Общий класс родитель///
class Entity {
public:
	float dx, dy, x, y, speed,moveTimer;//добавили переменную таймер для врага(в будущем)
	int w,h,health;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name;//враги могут быть разные, мы не будем делать другой класс для различающегося врага.всего лишь различим врагов по имени и дадим каждому свое действие в update в зависимости от имени
	Entity(Image &image, float X, float Y,int W,int H,String Name){
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}
};
/////КЛАСС ИГРОКА///
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;//добавляем тип перечисления - состояние объекта
	int playerScore;//эта переменная может быть только у игрока
 
	   Player(Image &image, float X, float Y,int W,int H,String Name):Entity(image,X,Y,W,H,Name){
		   playerScore = 0; state = stay;
		   if (name == "Player"){
			   sprite.setTextureRect(IntRect(20, 134, w, h));//спрайт откуда будет рисоваться персонаж
		   }
	   }
 
	   void control(){
		   if (Keyboard::isKeyPressed){//если нажата клавиша
			   if (Keyboard::isKeyPressed(Keyboard::Left)) {//а именно левая
				   state = left; speed = 0.1;
			   }
			   if (Keyboard::isKeyPressed(Keyboard::Right)) {
				   state = right; speed = 0.1;
			   }
 
			   if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//если нажата клавиша вверх и мы на земле, то можем прыгать
				   state = jump; dy = -0.6; onGround = false;//увеличил высоту прыжка
			   }
 
			   if (Keyboard::isKeyPressed(Keyboard::Down)) {
				   state = down;
			   }
		   }
	   } 
 
	   void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	   {
		   for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
		   for (int j = x / 32; j<(x + w) / 32; j++)
		   {
			   if (TileMap[i][j] == '0')//если элемент наш тайлик земли? то
			   {
				   if (Dy>0){ y = i * 32 - h;  dy = 0; onGround = true; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
				   if (Dy<0){ y = i * 32 + 32;  dy = 0; }//столкновение с верхними краями карты(может и не пригодиться)
				   if (Dx>0){ x = j * 32 - w; }//с правым краем карты
				   if (Dx<0){ x = j * 32 + 32; }// с левым краем карты
			   }
			   //else { onGround = false; }//надо убрать т.к мы можем находиться и на другой поверхности или платформе которую разрушит враг
		   }
	   }
	    
	   void update(float time)
	   {
		   control();//функция управления персонажем
		   switch (state)//тут делаются различные действия в зависимости от состояния
		   {
		   case right:dx = speed; break;//состояние идти вправо
		   case left:dx = -speed; break;//состояние идти влево

		   case stay: break;//и здесь тоже		
		   }
		   x += dx*time;
		   checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
		   y += dy*time;
		   checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
		   sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
		   if (health <= 0){ life = false; }
		   if (!isMove){ speed = 0; }
		   if (life) { setPlayerCoordinateForView(x, y); }
		   dy = dy + 0.0015*time;//постоянно притягиваемся к земле
	   } 
};
 
 
class Enemy :public Entity{

FloatRect getRect(){
		return FloatRect(x, y, w, h);
	}

public:
	Enemy(Image &image, float X, float Y,int W,int H,String Name):Entity(image,X,Y,W,H,Name){ 
		if (name == "EasyEnemy"){
			sprite.setTextureRect(IntRect(0, 0, w, h));
			dx = 0.1;//даем скорость.этот объект всегда двигается
		}
	}
 
	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
		for (int j = x / 32; j<(x + w) / 32; j++)
		{
			if (TileMap[i][j] == '0')//если элемент наш тайлик земли, то
			{
				if (Dy>0){ y = i * 32 - h; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
				if (Dy<0){ y = i * 32 + 32; }//столкновение с верхними краями карты(может и не пригодиться)
				if (Dx>0){ x = j * 32 - w; dx = -0.1; sprite.scale(-1, 1); }//с правым краем карты
				if (Dx<0){ x = j * 32 + 32; dx = 0.1; sprite.scale(-1, 1); }// с левым краем карты
			}
		}
	}
 
	void update(float time)
	{
		if (name == "EasyEnemy"){//для персонажа с таким именем логика будет такой
			
			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
			checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
			x += dx*time;
			sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
			if (health <= 0){ life = false; }
		}
	}
};
 void menu(RenderWindow & window) {
	Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
	menuTexture1.loadFromFile("images/111.png");
	menuTexture2.loadFromFile("images/222.png");
	menuTexture3.loadFromFile("images/333.png");
	aboutTexture.loadFromFile("images/about.png");
	menuBackground.loadFromFile("images/Scott.png");
	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBg(menuBackground);
	bool isMenu = 1;//логическая переменная (надо рисовать меню или нет)
	int menuNum = 0;
	menu1.setPosition(100, 30);
	menu2.setPosition(100, 90);
	menu3.setPosition(100, 150);
	menuBg.setPosition(0, 0);
 

	Music music;//создаем объект музыки
	music.openFromFile("menu.ogg");//загружаем файл
	music.play();//воспроизводим музыку
////МЕНЮ/////
	while (isMenu)
	{
		menu1.setColor(Color::White);//окрашеваем в белый возвращаем картинке её оригинальный цвет 
		menu2.setColor(Color::White);
		menu3.setColor(Color::White);
		menuNum = 0;
		window.clear(Color(129, 181, 221));
 
		if (IntRect(100, 30, 300, 50).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Blue); menuNum = 1; }// если мы наводим курсор на одну из табличек мы окрашиваем их в голубой цвет ,тем самым мы определяем на какой из элементов мы навели  
		if (IntRect(100, 90, 300, 50).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Blue); menuNum = 2; }
		if (IntRect(100, 150, 300, 50).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Blue); menuNum = 3; }
 
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) isMenu = false;//если нажали первую кнопку, то выходим из меню 
			if (menuNum == 2) { window.draw(about); window.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
			if (menuNum == 3)  { window.close(); isMenu = false; }
 
		}
 
		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);
		
		window.display();
	}
	////////////////////////////////////////////////////
}
int main()
{
RenderWindow window(sf::VideoMode(1173, 779), "8");
	menu(window);//вызов меню
	view.reset(FloatRect(0, 0, 640, 480));


 
	Music music;//создаем объект музыки
	music.openFromFile("action.wav");//загружаем файл
	music.play();//воспроизводим музыку	
	music.setLoop(true); 

	Image map_image;
	map_image.loadFromFile("images/map.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);
	
	Image heroImage;
	heroImage.loadFromFile("images/hero3.png");
 
	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images/Vrag.png");
	easyEnemyImage.createMaskFromColor(Color(255, 0, 0));//сделали маску по цвету
 
 
	Player p(heroImage, 750, 500,40,30,"Player");//объект класса игрока
	Enemy easyEnemy(easyEnemyImage, 850, 671,200,97,"EasyEnemy");//простой враг, объект класса врага
	
	Clock clock;
	while (window.isOpen())
	{
 
		float time = clock.getElapsedTime().asMicroseconds();
 
		clock.restart();
		time = time / 800;
		
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();	
		}		
		p.update(time);// Player update function
		
for (it = entities.begin(); it != entities.end();)//говорим что проходимся от начала до конца
		{
			Entity *b = *it;//для удобства, чтобы не писать (*it)->
			b->update(time);//вызываем ф-цию update для всех объектов (по сути для тех, кто жив)
			if (b->life == false)	{ it = entities.erase(it); delete b; }// если этот объект мертв, то удаляем его
			else it++;//и идем курсором (итератором) к след объекту. так делаем со всеми объектами списка
		}
for (it = entities.begin(); it != entities.end(); it++)//проходимся по эл-там списка
		{
			if ((*it)->getRect().intersects(p.getRect()))//если прямоугольник спрайта объекта пересекается с игроком
			{
				if ((*it)->name == "EasyEnemy"){//и при этом имя объекта EasyEnemy,то..
					
					if ((p.dy>0) && (p.onGround == false)) { (*it)->dx = 0; p.dy = -0.2; (*it)->health = 0; }//если прыгнули на врага,то даем врагу скорость 0,отпрыгиваем от него чуть вверх,даем ему здоровье 0
					else {
						p.health -= 5;	//иначе враг подошел к нам сбоку и нанес урон
					}
				}
			}
		}

		easyEnemy.update(time);//easyEnemy update function
		window.setView(view);
		window.clear();
 
		
			for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
				if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
				if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
				s_map.setPosition(j * 32, i * 32);
				
				window.draw(s_map);
			}
		window.draw(easyEnemy.sprite);
		window.draw(p.sprite);
		window.display();
	}
	return 0;
}