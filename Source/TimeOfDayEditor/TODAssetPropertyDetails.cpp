// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "TimeOfDayEditor.h"

#include "TODAssetPropertyDetails.h"

float FTODFloatCurveProperty::GetCurrentFloatValue() const
{
	if (PropertyHandle.IsValid())
	{
		UProperty* propPtr = PropertyHandle->GetProperty();
		UStructProperty* structProp = Cast<UStructProperty>(propPtr);
		if (structProp)
		{
			FRuntimeFloatCurve* floatCurve = structProp->ContainerPtrToValuePtr<FRuntimeFloatCurve>(TODAsset);
			if (floatCurve)
			{
				return floatCurve->EditorCurveData.Eval(TODAsset->Hour);
			}
		}
	}
	return 0;
}

float FTODFloatCurveProperty::GetFloatValueFromAttribute() const
{
	return GetFloatAttribute.Get();
}
void FTODFloatCurveProperty::ConstructWidget(IDetailCategoryBuilder& CategoryBuilder)
{
	
	CategoryBuilder.AddCustomRow(FString("Test Row"))
		[
			SNew(SProperty, PropertyHandle)
			.CustomWidget()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SNew(STextBlock)
						.Text(FText::FromString(PropertyHandle->GetPropertyDisplayName()))
					]
					+ SVerticalBox::Slot()
						[
							SNew(SSpinBox<float>)
							.MinSliderValue(0)
							.MaxSliderValue(1024)
							.Value(TAttribute<float>(this, &FTODFloatCurveProperty::GetFloatValueFromAttribute))
						]
				]

			]
		];
}
TSharedRef<IDetailCustomization> FTODAssetPropertyDetails::MakeInstance()
{
	return MakeShareable(new FTODAssetPropertyDetails());
}

FTODAssetPropertyDetails::~FTODAssetPropertyDetails()
{
	for (TSharedPtr<FTODFloatCurveProperty> floatCurve : FloatCurves)
	{
		floatCurve.Reset();
	}
	FloatCurves.Empty();
}

void FTODAssetPropertyDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	const IDetailsView& DetailView = DetailLayout.GetDetailsView();
	//first find asset we are going to edit.
	TWeakObjectPtr<UObject> InspectedObject;
	for (TWeakObjectPtr<UObject> inspObj : DetailView.GetSelectedObjects())
	{
		InspectedObject = inspObj;
		break;
	}

	UTODAsset* TODAsset = Cast<UTODAsset>(InspectedObject.Get());
	CurrentTODAsset = Cast<UTODAsset>(InspectedObject.Get());
	if (TODAsset)
	{
		for (TFieldIterator<UProperty> PropIt(TODAsset->GetClass()); PropIt; ++PropIt)
		{
			UProperty* prop = *PropIt;
			DetailLayout.HideProperty(prop->GetFName());
			//PropertyHandles.Add(DetailLayout.GetProperty(prop->GetFName()));
			UStructProperty* structProp = Cast<UStructProperty>(prop);
			if (structProp)
			{
				FRuntimeFloatCurve* floatCurve = structProp->ContainerPtrToValuePtr<FRuntimeFloatCurve>(TODAsset);
				if (floatCurve)
				{
					TSharedPtr<FTODFloatCurveProperty> tempFloatProp = MakeShareable(new FTODFloatCurveProperty());
					tempFloatProp->PropertyHandle = DetailLayout.GetProperty(prop->GetFName());
					tempFloatProp->TODAsset = TODAsset;
					tempFloatProp->CategoryName = tempFloatProp->PropertyHandle->GetMetaData(TEXT("Category"));
					FloatCurves.Add(tempFloatProp);
				}
			}

		}
	}
	IDetailCategoryBuilder& DetailCategoryBuilder = DetailLayout.EditCategory("Property Detail");
	FDetailWidgetRow& DetailRow = DetailCategoryBuilder.AddCustomRow(FString("Custom Row"));
	////now customize each property
	//FRuntimeFloatCurve* floatCurve;

	TSharedPtr<IPropertyHandle> hour = DetailLayout.GetProperty(TEXT("Hour"));
	DetailCategoryBuilder.AddProperty(hour);


	IDetailCategoryBuilder& SunCategoryBuilder = DetailLayout.EditCategory("Sun");
	IDetailCategoryBuilder& AFCategoryBuilder = DetailLayout.EditCategory("Atmospheric Fog");
	IDetailCategoryBuilder& HFCategoryBuilder = DetailLayout.EditCategory("Height Fog");
	IDetailCategoryBuilder& PPCategoryBuilder = DetailLayout.EditCategory("Post Process");
	IDetailCategoryBuilder& SkyLightCategoryBuilder = DetailLayout.EditCategory("SkyLight");
	IDetailCategoryBuilder& MoonCategoryBuilder = DetailLayout.EditCategory("Moon");
	for (TSharedPtr<FTODFloatCurveProperty> floatCurves : FloatCurves)
	{
		if (floatCurves->CategoryName == FString("Sun"))
			floatCurves->ConstructWidget(SunCategoryBuilder);

		if (floatCurves->CategoryName == FString("Atmospheric Fog"))
			floatCurves->ConstructWidget(AFCategoryBuilder);

		if (floatCurves->CategoryName == FString("Height Fog"))
			floatCurves->ConstructWidget(HFCategoryBuilder);

		if (floatCurves->CategoryName == FString("Post Process"))
			floatCurves->ConstructWidget(PPCategoryBuilder);

		if (floatCurves->CategoryName == FString("SkyLight"))
			floatCurves->ConstructWidget(SkyLightCategoryBuilder);

		if (floatCurves->CategoryName == FString("Moon"))
			floatCurves->ConstructWidget(MoonCategoryBuilder);
	}
}

float FTODAssetPropertyDetails::GetCurrentFloatValue() const
{
	if (SunIntensityCurve.IsValid())
	{
		UProperty* propPtr = SunIntensityCurve->GetProperty();
		UStructProperty* structProp = Cast<UStructProperty>(propPtr);
		FRuntimeFloatCurve* floatCurve = structProp->ContainerPtrToValuePtr<FRuntimeFloatCurve>(CurrentTODAsset);
		if (floatCurve)
		{
			return floatCurve->EditorCurveData.Eval(CurrentTODAsset->Hour);
		}
	}
	return 0;
}
float FTODAssetPropertyDetails::GetSunIntensityValue() const
{
	return GetSunIntensityCurveValue.Get();
}
TSharedPtr<SProperty> FTODAssetPropertyDetails::GetPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandleIn)
{
	return SNew(SProperty, PropertyHandleIn);
}

float FTODAssetPropertyDetails::GetFloatCurveValue(FRuntimeFloatCurve* FloatCurveIn, float TimeIn)
{
	return FloatCurveIn->EditorCurveData.Eval(TimeIn);
}
//virtual FText GetAssetDescription(const FAssetData& AssetData) const override;