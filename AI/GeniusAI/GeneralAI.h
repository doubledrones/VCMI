#ifndef __GENERAL_AI_H__
#define __GENERAL_AI_H__

#include "Common.h"

namespace geniusai { namespace GeneralAI {

	class CGeneralAI
	{
	public:
		CGeneralAI();
		~CGeneralAI();

		void init(CCallback* CB);
	private:
		CCallback *m_cb;
	};

}}

#endif/*__GENERAL_AI_H__*/