for varIndex = CoPVars.v_CoPx_MD:CoPVars.v_Area95
    for stateIndex = States.PureD:States.FCogEC
        meanValue = mean(Database(Subjects.NilouS:SubNum, stateIndex,varIndex));
        Database(Subjects.Mean, stateIndex, varIndex) = meanValue;
    end
end
