import pandas as pd
import statsmodels.api as sm
from statsmodels.stats.anova import AnovaRM

# === CONFIGURE THIS ===
excel_path = 'DatabaseV5.xlsx'
output_path = 'anova_results_statsmodels.csv'

# NEW descriptive state names matching your columns order in Excel sheets
state_cols = [
    'Rigid Surface', 
    'Rigid + Cog', 
    'Rigid + EC', 
    'Rigid + Cog + EC', 
    'Foam Surface', 
    'Foam + Cog', 
    'Foam + EC', 
    'Foam + Cog + EC'
]

xls = pd.ExcelFile(excel_path)
all_results = []

for sheet in xls.sheet_names:
    print(f"\n🔍 Processing sheet: {sheet}")

    # Read sheet with no header, assign descriptive state names as columns
    df_wide = pd.read_excel(xls, sheet_name=sheet, header=None)
    df_wide.columns = state_cols

    # Add Participant IDs (1 to number of rows)
    df_wide.insert(0, 'Participant', range(1, len(df_wide) + 1))

    # Melt wide to long format
    df_long = df_wide.melt(id_vars='Participant', var_name='State', value_name='Score')

    # Extract factors from new descriptive state names
    def extract_factors(state):
        if not isinstance(state, str):
            return pd.Series([None, None, None])
        
        surface = 'Foam' if 'Foam' in state else 'Rigid'
        cognitive = 'Yes' if 'Cog' in state else 'No'
        vision = 'Closed' if 'EC' in state else 'Open'
        return pd.Series([surface, vision, cognitive])

    df_long[['Surface', 'Vision', 'Cognitive']] = df_long['State'].apply(extract_factors)

    # Drop any rows with missing factor info (just in case)
    df_long = df_long.dropna(subset=['Surface', 'Vision', 'Cognitive'])

    # Convert factors to categorical dtype (required by statsmodels)
    for col in ['Surface', 'Vision', 'Cognitive']:
        df_long[col] = df_long[col].astype('category')

    # Run 3-way repeated measures ANOVA
    aov = AnovaRM(
        data=df_long,
        depvar='Score',
        subject='Participant',
        within=['Surface', 'Vision', 'Cognitive']
    ).fit()

    print(aov)

    # Save results, add measure name for reference
    aov_table = aov.anova_table
    aov_table['Measure'] = sheet
    all_results.append(aov_table.reset_index())

# Combine all sheet results and save as CSV
final_df = pd.concat(all_results, ignore_index=True)
final_df.to_csv(output_path, index=False)

print(f"\n✅ All ANOVA results saved to {output_path}")
