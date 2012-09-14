#include "baseapp.hpp"
#include "archiver.hpp"
#include "base.hpp"

namespace KBEngine{	

//-------------------------------------------------------------------------------------
Archiver::Archiver():
	archiveIndex_(INT_MAX),
	backupEntityIDs_()
{
}

//-------------------------------------------------------------------------------------
Archiver::~Archiver()
{
}

//-------------------------------------------------------------------------------------
void Archiver::tick()
{
	int32 periodInTicks = secondsToTicks(ServerConfig::getSingleton().getBaseApp().archivePeriod, 0);
	if (periodInTicks == 0)
		return;

	if (archiveIndex_ >= periodInTicks)
	{
		this->createArchiveTable();
	}

	// �㷨����:
	// base������ * idx / tick���� = ÿ����vector���ƶ���һ������
	// ���������ÿ��gametick���д���, �պ�ƽ������periodInTicks�д���������
	// ���archiveIndex_ >= periodInTicks�����²���һ���������
	int size = backupEntityIDs_.size();
	int startIndex = size * archiveIndex_ / periodInTicks;

	++archiveIndex_;

	int endIndex   = size * archiveIndex_ / periodInTicks;

	for (int i = startIndex; i < endIndex; ++i)
	{
		Base * pBase = Baseapp::getSingleton().findEntity(backupEntityIDs_[i]);
		
		if(pBase && pBase->hasDB())
		{
			this->archive(*pBase);
		}
	}
}

//-------------------------------------------------------------------------------------
void Archiver::archive(Base& base)
{
	base.writeToDB();
}

//-------------------------------------------------------------------------------------
void Archiver::createArchiveTable()
{
	archiveIndex_ = 0;
	backupEntityIDs_.clear();

	Entities<Base>::ENTITYS_MAP::const_iterator iter = Baseapp::getSingleton().pEntities()->getEntities().begin();

	for(; iter != Baseapp::getSingleton().pEntities()->getEntities().end(); iter++)
	{
		if(static_cast<Base*>(iter->second.get())->hasDB())
		{
			backupEntityIDs_.push_back(iter->first);
		}
	}

	// ���һ������
	std::random_shuffle(backupEntityIDs_.begin(), backupEntityIDs_.end());
}

}