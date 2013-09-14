#include "surveillancemonitor.h"
#include "team.h"
#include "basedoor.h"

SurveillanceMonitor::SurveillanceMonitor() : Object(ObjectTypes::SURVEILLANCEMONITOR){
	res_bank = 65;
	res_index = 0;
	renderxoffset = 0;
	renderyoffset = 0;
	objectfollowing = 0;
	teamid = 0;
	surveillancecamera = -1;
	// 65 is screen borders 0:large 1:small 2:base screen
}

void SurveillanceMonitor::Serialize(bool write, Serializer & data, Serializer * old){
	Object::Serialize(write, data, old);
}

void SurveillanceMonitor::Tick(World & world){
	if(teamid){
		if(!objectfollowing){
			Team * team = static_cast<Team *>(world.GetObjectFromId(teamid));
			if(team){
				BaseDoor * basedoor = static_cast<BaseDoor *>(world.GetObjectFromId(team->basedoorid));
				if(basedoor){
					objectfollowing = basedoor->id;
				}
			}
		}
	}else{
		if(world.map.surveillancecameras.size() > 0){
			if(rand() % 500 == 0){
				surveillancecamera = -1;
			}
			if(surveillancecamera == -1){
				surveillancecamera = rand() % world.map.surveillancecameras.size();
			}
			camera.SetPosition(world.map.surveillancecameras[surveillancecamera].x, world.map.surveillancecameras[surveillancecamera].y);
		}
	}
	/*if(!objectfollowing){
		if(teamid){
			Team * team = static_cast<Team *>(world.GetObjectFromId(teamid));
			if(team){
				BaseDoor * basedoor = static_cast<BaseDoor *>(world.GetObjectFromId(team->basedoorid));
				if(basedoor){
					objectfollowing = basedoor->id;
				}
			}
		}else{
			int index = rand() % world.objectlist.size();
			int i = 0;
			for(std::list<Object *>::iterator it = world.objectlist.begin(); it != world.objectlist.end(); it++, i++){
				if(i == index){
					objectfollowing = (*it)->id;
					break;
				}
			}
		}
	}*/
	if(objectfollowing){
		//if(rand() % 500 == 0){
		//	objectfollowing = 0;
		//}
		Object * object = world.GetObjectFromId(objectfollowing);
		if(object){
			camera.SetPosition(object->x, object->y - (world.resources.spriteoffsety[object->res_bank][object->res_index] / 2));
		}else{
			objectfollowing = 0;
		}
	}
}

void SurveillanceMonitor::SetSize(Uint8 size){
	res_index = size;
	switch(size){
		default:
		case 0:
			res_index = 0;
			renderxoffset = 8;
			renderyoffset = 11;
			camera.w = 99 * 2;
			camera.h = 97 * 2;
		break;
		case 1:
			res_index = 1;
			renderxoffset = 6;
			renderyoffset = 7;
			camera.w = 78 * 2;
			camera.h = 41 * 2;
		break;
		case 2:
			res_index = 2;
			renderxoffset = 5;
			renderyoffset = 5;
			camera.w = 101 * 2;
			camera.h = 125 * 2;
		break;
	}
}