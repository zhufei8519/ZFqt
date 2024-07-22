
#ifndef ZFqt_TREE_WIDGET_H
#define ZFqt_TREE_WIDGET_H

#include "ZFqt/types.h"

#include <QTreeWidget>

namespace ZFqt
{
	struct TreeWidgetItemData
	{
		TreeWidgetItemData()
			:uNodeType(ZFqt::Const_InvalidID)
		{}
		TreeWidgetItemData(uint32_t uNodeTypeParam)
			:uNodeType(uNodeTypeParam)
		{}
		virtual ~TreeWidgetItemData()
		{}

		uint32_t	uNodeType;
	};

	class TreeWidgetItem: public QTreeWidgetItem
	{
	public:
		TreeWidgetItem()
			:QTreeWidgetItem(QTreeWidgetItem::UserType)
			, m_pItemData(NULL)
		{}
		virtual ~TreeWidgetItem()
		{
			if (this->m_pItemData != NULL)
			{
				delete this->m_pItemData;
				this->m_pItemData	=	NULL;
			}
		}

		void	SetItemData(TreeWidgetItemData* pItemData)	{ this->m_pItemData = pItemData; }
		TreeWidgetItemData*	GetItemData()					{ return this->m_pItemData; }

	protected:
		TreeWidgetItemData* m_pItemData;
	};

	class TreeWidget: public QTreeWidget
	{
	public:
		TreeWidget(QWidget* parent = nullptr)
			:QTreeWidget(parent)
		{}
		virtual ~TreeWidget()	{}

		virtual void	Clear()	{ this->clear(); }

		uint32_t	GetItemNodeType(QTreeWidgetItem* pItem) const
		{
			if (pItem != NULL)
			{
				TreeWidgetItemData*	pItemData	=	dynamic_cast< TreeWidgetItem* >(pItem)->GetItemData();
				if (pItemData != NULL)
				{
					return pItemData->uNodeType;
				}
			}

			return ZFqt::Const_InvalidID;
		}
		uint32_t	CurrentItemNodeType() const	{ return this->GetItemNodeType(this->currentItem()); }
	};
};

#endif //end ZFqt_TREE_WIDGET_H

