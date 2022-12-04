/*
 * ��������Ʈ �̺�Ʈ���� ���縦 ���� ����ؼ� �ȵ�.
 * �̺�Ʈ���� ������ ���̵� ��� �ִµ� �̴� �ڵ鷯�� ���� ���Ÿ� ������.
 * ���� ��������Ʈ �̺�Ʈ�� ���簡 �ȴٸ� �� ��������Ʈ �ȿ� �ߺ��� ���̵� ���� ��������Ʈ �̺�Ʈ�� ���� ���� ����.
 * ���� ��������Ʈ �̺�Ʈ���� ���̵� �����ϴ� ��������Ʈ ���忡���� ġ����.
 *
 * �̰� �����غ��ϱ� ����� ���Ƶ� �̵��� ����ؾ� �ҵ�.
 */

#pragma once

#include <unordered_set>
#include "TypeDefine.h"

namespace joo
{
	/**
	 * @brief �⺻ ��������Ʈ �̺�Ʈ Ÿ���Դϴ�.
	 * @tparam TRet ��ȯ Ÿ��.
	 * @tparam TParams �Է� ���� ���� Ÿ��.
	 */
	template <typename TRet, typename... TParams>
	class JDelegateEvent
	{
	public:
		JDelegateEvent()
			: mEventId(generateDelegateEventId())
			, mbIsExpired(false)
		{
		}

		/**
		 * @brief ��������Ʈ �̺�Ʈ�� ȣ���մϴ�.
		 * @warning Invocation �Ŀ� ���� ���θ� �����մϴ�.
		 * @return Invocation�� ����� ��ȯ. Invocation�� �����ϸ� �⺻ �����ڸ� ��ȯ�մϴ�.
		 * @warning TRet�� �⺻ �����ڰ� �ݵ�� ���ǵǾ� �־�� �մϴ�.
		 */
		virtual TRet Invoke(TParams ... params) = 0;

		/**
		 * @brief ��������Ʈ �̺�Ʈ�� ����Ǿ����� Ȯ���մϴ�.
		 * @details ���� ���δ� Invocation �Ŀ� �� �� �ֽ��ϴ�.
		 * @return ��������Ʈ �̺�Ʈ ���� ����.
		 */
		inline bool IsExpired() const
		{
			return mbIsExpired;
		}

		/**
		 * @brief ��������Ʈ�� �ο��� ��������Ʈ �̺�Ʈ ���̵� ��ȯ�մϴ�.
		 * @return ��������Ʈ �̺�Ʈ ���̵�.
		 */
		inline DelegateEventId GetEventId() const
		{
			return mEventId;
		}

		bool operator==(const JDelegateEvent& rhs)
		{
			return mEventId == rhs.mEventId;
		}

	protected:
		const DelegateEventId mEventId;
		bool mbIsExpired;

	private:
		DelegateEventId generateDelegateEventId() const
		{
			return reinterpret_cast<DelegateEventId>(this);
		}

	public:
		virtual ~JDelegateEvent() = default;
#pragma region Deleted Default Methods
		JDelegateEvent(const JDelegateEvent& other) = delete;
		JDelegateEvent(JDelegateEvent&& other) = delete;
		JDelegateEvent& operator=(const JDelegateEvent& rhs) = delete;
		JDelegateEvent& operator=(JDelegateEvent&& rhs) = delete;
#pragma endregion
	};
}

namespace std
{
	template <typename TRet, typename... TParams>
	struct hash<joo::JDelegateEvent<TRet, TParams...>>
	{
		size_t operator()(const joo::JDelegateEvent<TRet, TParams...>& event) const
		{
			static_assert(sizeof(size_t) == sizeof(joo::JDelegateEvent<TRet, TParams...>::mEventId));
			return event.GetEventId();
		}
	};
}
