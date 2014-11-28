#pragma once
#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetTypeActions_Base.h"
#include "Editor/PropertyEditor/Public/IDetailCustomNodeBuilder.h"

/*
	Customization for curve property display.

	Instead of displaying Curve, it will display name of property and edit box with value of
	of curve in current time.
	

	If there is no key in current value and value will be changed, new key will be added to curve
	at current time.
*/
class FTODProperty
{
public:
	FTODProperty()
	{
		//CategoryBuilder = nullptr;
	}
	TSharedPtr<IPropertyHandle> PropertyHandle;

	TAttribute<float> GetFloatAttribute;

	UTODAsset* TODAsset;

	FString CategoryName;
};
class FTODFloatCurveProperty : public FTODProperty, public TSharedFromThis<FTODFloatCurveProperty>
{
public:
	//FRuntimeFloatCurve* FloatCurve;
	FTODFloatCurveProperty()
	{

		//FloatCurve = nullptr;
		TODAsset = nullptr;
		GetFloatAttribute.BindRaw(this, &FTODFloatCurveProperty::GetCurrentFloatValue);
	}

	~FTODFloatCurveProperty()
	{
		//FloatCurve = nullptr;
		TODAsset = nullptr;
	}

	float GetCurrentFloatValue() const;

	float GetFloatValueFromAttribute() const;

	void ConstructWidget(IDetailCategoryBuilder& CategoryBuilder);
};

class FTODAssetPropertyDetails : public IDetailCustomization
{
public:
	FTODAssetPropertyDetails()
	{};
	~FTODAssetPropertyDetails();
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

protected:
	TArray<TSharedPtr<IPropertyHandle>> PropertyHandles;

	TSharedPtr<SProperty> GetPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandleIn);

	float GetFloatCurveValue(FRuntimeFloatCurve* FloatCurveIn, float TimeIn);

	TArray<TSharedPtr<FTODFloatCurveProperty>> FloatCurves;

	TSharedPtr<IPropertyHandle> SunIntensityCurve;
	TAttribute<float> GetSunIntensityCurveValue;
	float GetSunIntensityValue() const;

	float GetCurrentFloatValue() const;

	class UTODAsset* CurrentTODAsset;
};