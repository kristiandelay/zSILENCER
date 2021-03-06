#include "projectile.h"
#include "object.h"
#include "walldefense.h"
#include "player.h"
#include "robot.h"
#include "fixedcannon.h"
#include "flamerprojectile.h"
#include "flareprojectile.h"

Projectile::Projectile(){
	shielddamage = 0;
	healthdamage = 0;
	//renderpass = 2;
	velocity = 25;
	emitoffset = 0;
	bypassshield = false;
	moveamount = 0;
	radius = 0;
	moving = false;
	stopatobjectcollision = true;
}

void Projectile::Serialize(bool write, Serializer & data, Serializer * old){
	data.Serialize(write, ownerid, old);
}

bool Projectile::TestCollision(Object & object, World & world, Platform ** collidedplatform, Object ** collidedobject){
	moving = true;
	int xe = object.xv;
	int ye = object.yv;
	Object * owner = world.GetObjectFromId(ownerid);
	std::vector<Uint8> types;
	/*bool issecurity = false;
	if(owner && (owner->type == ObjectTypes::GUARD || owner->type == ObjectTypes::ROBOT || owner->type == ObjectTypes::WALLDEFENSE)){
		if(owner->type == ObjectTypes::WALLDEFENSE){
			WallDefense * walldefense = static_cast<WallDefense *>(owner);
			if(!walldefense->teamid){
				issecurity = true;
			}
		}else
		if(owner->type == ObjectTypes::ROBOT){
			Robot * robot = static_cast<Robot *>(owner);
			if(!robot->virusplanter){
				issecurity = true;
			}
		}else{
			issecurity = true;
		}
	}*/
	if(owner && world.IsSecurity(*owner)){
		types.push_back(ObjectTypes::PLAYER);
		types.push_back(ObjectTypes::CIVILIAN);
		types.push_back(ObjectTypes::FIXEDCANNON);
	}else{
		types.push_back(ObjectTypes::PLAYER);
		types.push_back(ObjectTypes::CIVILIAN);
		types.push_back(ObjectTypes::GUARD);
		types.push_back(ObjectTypes::ROBOT);
		types.push_back(ObjectTypes::FIXEDCANNON);
		types.push_back(ObjectTypes::TECHSTATION);
		if(object.type != ObjectTypes::WALLPROJECTILE){
			types.push_back(ObjectTypes::WALLDEFENSE);
		}
	}
	Uint16 teamid = 0;
	if(owner){
		switch(owner->type){
			case ObjectTypes::PLAYER:{
				Player * player = static_cast<Player *>(owner);
				Team * team = player->GetTeam(world);
				if(team){
					teamid = team->id;
				}
			}break;
			case ObjectTypes::ROBOT:{
				Robot * robot = static_cast<Robot *>(owner);
				teamid = robot->virusplanter;
			}break;
			case ObjectTypes::FIXEDCANNON:{
				FixedCannon * fixedcannon = static_cast<FixedCannon *>(owner);
				teamid = fixedcannon->teamid;
			}break;
			case ObjectTypes::WALLDEFENSE:{
				WallDefense * walldefense = static_cast<WallDefense *>(owner);
				teamid = walldefense->teamid;
			}break;
		}
	}
	Uint16 skipobject = ownerid;
	bool poisonous = false;
	if(object.type == ObjectTypes::FLAREPROJECTILE){
		FlareProjectile & flareprojectile = static_cast<FlareProjectile &>(object);
		poisonous = flareprojectile.poisonous;
	}
	if(object.type == ObjectTypes::PLASMAPROJECTILE || object.type == ObjectTypes::FLAREPROJECTILE){
		skipobject = 0;
		teamid = 0;
	}
	Object * thecollidedobject = world.TestIncr(object.x - radius, object.y - radius, object.x + radius, object.y + radius, &xe, &ye, types, skipobject, teamid);
	bool collided = false;
	if(thecollidedobject){
		object.x += xe;
		object.y += ye;
		if(thecollidedobject->issprite){
			int hx = (object.x - (thecollidedobject->x)) + world.resources.spriteoffsetx[thecollidedobject->res_bank][thecollidedobject->res_index];
			if(thecollidedobject->mirrored){
				hx = (object.x - (thecollidedobject->x)) + (world.resources.spritewidth[thecollidedobject->res_bank][thecollidedobject->res_index] - world.resources.spriteoffsetx[thecollidedobject->res_bank][thecollidedobject->res_index]);
			}
			int hy = (object.y - (thecollidedobject->y)) + world.resources.spriteoffsety[thecollidedobject->res_bank][thecollidedobject->res_index];
			if(hx < 0){
				hx = 0;
			}
			if(hy < 0){
				hy = 0;
			}
			if(hx > world.resources.spritewidth[thecollidedobject->res_bank][thecollidedobject->res_index]){
				hx = world.resources.spritewidth[thecollidedobject->res_bank][thecollidedobject->res_index];
			}
			if(hy > world.resources.spriteheight[thecollidedobject->res_bank][thecollidedobject->res_index]){
				hy = world.resources.spriteheight[thecollidedobject->res_bank][thecollidedobject->res_index];
			}
			hx = (float(hx) / world.resources.spritewidth[thecollidedobject->res_bank][thecollidedobject->res_index]) * 100;
			if(thecollidedobject->mirrored){
				hx = 100 - hx;
			}
			hy = (float(hy) / world.resources.spriteheight[thecollidedobject->res_bank][thecollidedobject->res_index]) * 100;
			thecollidedobject->HandleHit(world, hx, hy, object);
			Object * owner = world.GetObjectFromId(ownerid);
			if(owner && owner->type == ObjectTypes::PLAYER){
				Player * player = static_cast<Player *>(owner);
				Peer * peer = player->GetPeer(world);
				if(thecollidedobject->type == ObjectTypes::PLAYER){
					Player * collidedplayer = static_cast<Player *>(thecollidedobject);
					if(poisonous /*&& collidedplayer->id != ownerid*/){
						// poison a little bit 4 times per second, max of 3 poison
						if(world.tickcount % 6 == 0 && collidedplayer->poisonedamount < 3 && collidedplayer->Poison(world, ownerid, 1)){
							/*if(peer){
								peer->stats.poisons++;
							}*/
						}
					}
				}
				if(peer){
					switch(object.type){
						case ObjectTypes::BLASTERPROJECTILE:{
							peer->stats.weaponhits[0]++;
						}break;
						case ObjectTypes::LASERPROJECTILE:{
							peer->stats.weaponhits[1]++;
						}break;
						case ObjectTypes::ROCKETPROJECTILE:{
							peer->stats.weaponhits[2]++;
						}break;
						case ObjectTypes::FLAMERPROJECTILE:{
							FlamerProjectile * flamerprojectile = static_cast<FlamerProjectile *>(&object);
							if(!flamerprojectile->hitonce){
								flamerprojectile->hitonce = true;
								peer->stats.weaponhits[3]++;
							}
						}break;
					}
				}
			}
		}
		if(collidedobject){
			*collidedobject = thecollidedobject;
		}
		collided = true;
	}
	xe = object.xv;
	ye = object.yv;
	Platform * thecollidedplatform = world.map.TestIncr(object.x, object.y, object.x, object.y, &xe, &ye, Platform::RECTANGLE | Platform::STAIRSUP | Platform::STAIRSDOWN, 0, true);
	if(!thecollidedobject || !stopatobjectcollision){
		object.x += xe;
		object.y += ye;
	}
	if(thecollidedplatform){
		if(collidedplatform){
			*collidedplatform = thecollidedplatform;
		}
		collided = true;
	}
	return collided;
}