#pragma once

// Safely call an interface function if the object implements it.
template<typename TInterface, typename TObject>
FORCEINLINE void SafeInterfaceCall(TObject* Object, void(TInterface::*Func)())
{
	if (!Object) return;

	if (TInterface* Interface = Cast<TInterface>(Object))
	{
		(Interface->*Func)();
	}
}
